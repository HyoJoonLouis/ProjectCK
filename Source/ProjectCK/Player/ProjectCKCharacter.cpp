#include "ProjectCKCharacter.h"
#include "../ProjectCKGameInstance.h"
#include "../Components/DamageSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AProjectCKCharacter::AProjectCKCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true; 
	CameraBoom->SocketOffset = FVector(0, 40, 0);
	CameraBoom->TargetOffset = FVector(0, 0, 75);
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->FieldOfView = 78;

	DamageSystemComponent = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystem"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
}

// DamagableInterface
float AProjectCKCharacter::GetCurrentHealth_Implementation()
{
	return DamageSystemComponent->CurrentHealth;
}

float AProjectCKCharacter::GetMaxHealth_Implementation()
{
	return DamageSystemComponent->MaxHealth;
}

void AProjectCKCharacter::Heal_Implementation(float Amount)
{
	DamageSystemComponent->Heal(Amount);
}

bool AProjectCKCharacter::TakeDamage_Implementation(AActor* CauseActor, FDamageInfo DamageInfo)
{
	GetMesh()->GetAnimInstance()->Montage_Play(TakeDamageMontage, 2.0f);
	return DamageSystemComponent->TakeDamage(DamageInfo);
}

bool AProjectCKCharacter::IsDead_Implementation()
{
	return DamageSystemComponent->IsDead;
}

bool AProjectCKCharacter::IsAttacking_Implementation()
{
	return CheckCurrentState(EPlayerStates::ATTACKING);
}

bool AProjectCKCharacter::ReserveAttackToken_Implementation(int Amount)
{
	return DamageSystemComponent->ReserveAttackToken(Amount);
}

void AProjectCKCharacter::ReturnAttackToken_Implementation(int Amount)
{
	DamageSystemComponent->ReturnAttackToken(Amount);
}

void AProjectCKCharacter::SetState(EPlayerStates NewState)
{
	if (CurrentState == NewState)
		return;
	CurrentState = NewState;
}

void AProjectCKCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Input
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// HUD
	if (HUDClass)
	{
		HUD = Cast<UUserWidget>(CreateWidget(GetWorld(), HUDClass));
		if(HUD)
			HUD->AddToViewport();
	}
}

void AProjectCKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectCKCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectCKCharacter::Look);
		
		EnhancedInputComponent->BindAction(LeftAttackAction, ETriggerEvent::Started, this, &AProjectCKCharacter::LeftAttack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("Error In PlayerSetupPlayerInputComponent"));
	}
}

void AProjectCKCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProjectCKCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProjectCKCharacter::LeftAttack(const FInputActionValue& Value)
{
	if (CheckCurrentState(EPlayerStates::ATTACKING) || LeftAttackMontages.Num() == 0)
		return;

	SetState(EPlayerStates::ATTACKING);
	SetAttackTarget();
	if (TargetActor && FVector::Distance(TargetActor->GetActorLocation(), GetActorLocation()) > 500)
	{
		FMotionWarpingTarget Target = {};
		Target.Name = FName("AttackTarget");
		Target.Location = TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal() * 100;
		Target.Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());
		MotionWarpingComponent->AddOrUpdateWarpTarget(Target);
		PlayAnimMontage(MotionWarpingMontage);
		AttackIndex = 0;
		//SetActorLocation(TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal() * 100);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).Normalize() * 100).ToString());
	}
	else
	{
		PlayAnimMontage(LeftAttackMontages[AttackIndex]);
		AttackIndex++;
		if (AttackIndex >= LeftAttackMontages.Num())
			AttackIndex = 0;
	}
}

void AProjectCKCharacter::StartWeaponCollision()
{
	AlreadyHitActors.Empty();
}

void AProjectCKCharacter::TickWeaponCollision()
{
	FCollisionShape Sphere = FCollisionShape::MakeSphere(20.0f);
	TArray<FHitResult> OutResults;
}

void AProjectCKCharacter::EndWeaponCollision()
{
	SetState(EPlayerStates::NONE);
}

void AProjectCKCharacter::SetAttackTarget()
{
	// First Draw A Sphere Infront of Player
	FVector StartPos = GetActorLocation() + GetActorForwardVector() * 100;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());

	FHitResult HitResult;

	//bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPos, StartPos, 50.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);

	//if (Result)
	//{
	//	TargetActor = HitResult.GetActor();
	//}
	//else
	//{
		// Draw Sphere by Input
		FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
		StartPos = GetActorLocation() + LastInputVector * 100;
		FVector EndPos = StartPos + LastInputVector * 500;
		bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPos, EndPos, 100.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true, FLinearColor::Black);

		if (Result)
		{
			TargetActor = HitResult.GetActor();
		}
	//}
}