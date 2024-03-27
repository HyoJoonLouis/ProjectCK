#include "ProjectCKCharacter.h"
#include "../ProjectCKGameInstance.h"
#include "../Components/DamageSystemComponent.h"
#include "../Structs/EnumTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "InputActionValue.h"
#include "Engine/SkeletalMeshSocket.h"
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

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("WeaponSocket"));
	Weapon->SetCollisionProfileName(FName("NoCollision"), true);


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

	MovementSpeed.Add(EMovementSpeed::IDLE, 0);
	MovementSpeed.Add(EMovementSpeed::WALKING, 600);
	MovementSpeed.Add(EMovementSpeed::SPRINTING, 1200);

	DamageSystemComponent = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystem"));
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
	if (DamageSystemComponent->IsInvincible)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(DodgeStopTimer))
			TimerManager.ClearTimer(DodgeStopTimer);

		// Sphere Trace Based
		// Change Later
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypes.Add(Pawn);
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		TArray<FHitResult> HitResults;
		bool Result = UKismetSystemLibrary::SphereTraceMultiForObjects(this, GetActorLocation(), GetActorLocation(), 1000.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResults, true);
		if (Result)
		{
			for (const auto& HitResult : HitResults)
			{
				HitResult.GetActor()->CustomTimeDilation = 0.1f;
				DilationActors.AddUnique(HitResult.GetActor());
			}
		}

		TimerManager.SetTimer(DodgeStopTimer, [&]() {
			for (const auto& DilationActor : DilationActors)
			{
				DilationActor->CustomTimeDilation = 1;
			}
			DilationActors.Empty();
		}, 5.0f, false);
	}
	else 
	{
		GetMesh()->GetAnimInstance()->Montage_Play(TakeDamageMontage, 2.0f);
	}
	return DamageSystemComponent->TakeDamage(DamageInfo);
}
 
bool AProjectCKCharacter::IsDead_Implementation()
{
	return DamageSystemComponent->IsDead;
}

bool AProjectCKCharacter::IsAttacking_Implementation()
{
	return CheckCurrentState({EPlayerStates::ATTACKING});
}

bool AProjectCKCharacter::ReserveAttackToken_Implementation(int Amount)
{
	return DamageSystemComponent->ReserveAttackToken(Amount);
}

void AProjectCKCharacter::ReturnAttackToken_Implementation(int Amount)
{
	DamageSystemComponent->ReturnAttackToken(Amount);
}

void AProjectCKCharacter::ChangeState(EPlayerStates NewState)
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
	
	// Timeline
	if (TargetRotateCurve)
	{
		FOnTimelineFloat TargetRotateCurveCallback;
		TargetRotateCurveCallback.BindUFunction(this, FName("RotateToTargetTimelineFunction"));

		TargetRotateTimeline.AddInterpFloat(TargetRotateCurve, TargetRotateCurveCallback);
		TargetRotateTimeline.SetTimelineLength(0.2f);
	}

	// HUD
	if (HUDClass)
	{
		HUD = Cast<UUserWidget>(CreateWidget(GetWorld(), HUDClass));
		if(HUD)
			HUD->AddToViewport();
	}
}

void AProjectCKCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Timeline
	TargetRotateTimeline.TickTimeline(DeltaTime);

	// Weapon Collision
	// Better putting in C++ than in BP
	if (bActivateCollision)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypes.Add(Pawn);
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		TArray<FHitResult> HitResults;
		bool Result = UKismetSystemLibrary::SphereTraceMultiForObjects(this, Weapon->GetSocketLocation(FName("Start")), Weapon->GetSocketLocation(FName("End")), 30.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, HitResults, true);

		if (Result)
		{
			for (const auto& HitResult : HitResults)
			{
				if (AlreadyHitActors.Contains(HitResult.GetActor()))
					continue;

				AlreadyHitActors.AddUnique(HitResult.GetActor());
				
				FTimerManager& TimerManager = GetWorld()->GetTimerManager();
				if (TimerManager.IsTimerActive(HitStopTimer))
					TimerManager.ClearTimer(HitStopTimer);
				CustomTimeDilation = 0;
				TimerManager.SetTimer(HitStopTimer, [&]() {
						CustomTimeDilation = 1;
					}, 0.03f, false);

				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(SmallCameraShake);

				auto HitDamageInterface = Cast<IDamagableInterface>(HitResult.GetActor());
				if (HitDamageInterface)
				{
					HitDamageInterface->Execute_TakeDamage(HitResult.GetActor(), this, CurrentDamageInfo);
				}
			}
		}

	}

	// Face to Target
	if (TargetActor && FVector::Distance(GetActorLocation(), TargetActor->GetActorLocation()) <= 500)
	{
		Controller->SetControlRotation(UKismetMathLibrary::RInterpTo(Controller->GetControlRotation(), UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation() - FVector(0, 0, 30)), DeltaTime, 5));
	}
	else
	{
		TargetActor = NULL;
	}

}

void AProjectCKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectCKCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectCKCharacter::Look);
		
		EnhancedInputComponent->BindAction(LeftAttackAction, ETriggerEvent::Started, this, &AProjectCKCharacter::LeftMouse);
	
		EnhancedInputComponent->BindAction(RightAttackAction, ETriggerEvent::Started, this, &AProjectCKCharacter::RightMouse);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AProjectCKCharacter::Dodge);

		EnhancedInputComponent->BindAction(TargetingAction, ETriggerEvent::Started, this, &AProjectCKCharacter::Targeting);
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AProjectCKCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AProjectCKCharacter::StopSprint);
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
	if (TargetActor)
		return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProjectCKCharacter::LeftMouse(const FInputActionValue& Value)
{
	bSaveDodge = false;
	if (CheckCurrentState({ EPlayerStates::ATTACKING, EPlayerStates::DODGE }))
	{
		AttackInfo.AttackType = EAttackType::LEFT;
		AttackInfo.AttackSaved = true;
	}
	else
	{
		Attack(EAttackType::LEFT);

	}
}

void AProjectCKCharacter::RightMouse(const FInputActionValue& Value)
{
	bSaveDodge = false;
	if (CheckCurrentState({ EPlayerStates::ATTACKING, EPlayerStates::DODGE }))
	{
		AttackInfo.AttackType = EAttackType::RIGHT;
		AttackInfo.AttackSaved = true;
	}
	else
	{
		Attack(EAttackType::RIGHT);
	}
}

void AProjectCKCharacter::Dodge(const FInputActionValue& Value)
{
	if (!CheckCurrentState({ EPlayerStates::DODGE }) && !GetCharacterMovement()->IsFalling())
	{
		PerformDodge();
	}
	else
	{
		bSaveDodge = true;
	}
}

void AProjectCKCharacter::Targeting(const FInputActionValue& Value)
{
	if (!TargetActor)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypes.Add(Pawn);
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		FHitResult HitResult;

		APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(), GetActorLocation() + CameraManager->GetActorForwardVector() * 500, 100, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);
		if (Result)
		{
			TargetActor = HitResult.GetActor();
		}
	}
}

void AProjectCKCharacter::Sprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed[EMovementSpeed::SPRINTING];
}

void AProjectCKCharacter::StopSprint(const FInputActionValue& Value)
{ 
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed[EMovementSpeed::WALKING];
}

void AProjectCKCharacter::Attack(EAttackType AttackType)
{
	if (CheckCurrentState({ EPlayerStates::ATTACKING }))
		return;

	if (AttackType == EAttackType::LEFT)
		PerformLeftAttack(AttackIndex);
	else if (AttackType == EAttackType::RIGHT)
		PerformRightAttack(AttackIndex);
}

void AProjectCKCharacter::PerformLeftAttack(int Index)
{
	ChangeState(EPlayerStates::ATTACKING);
	SoftLock();
	PlayAnimMontage(LeftAttackMontages[Index]);
	AttackIndex++;
	if (AttackIndex >= LeftAttackMontages.Num())
		AttackIndex = 0;
}

void AProjectCKCharacter::PerformRightAttack(int Index)
{
	ChangeState(EPlayerStates::ATTACKING);
	SoftLock(); 
	PlayAnimMontage(RightAttackMontages[Index]);
	AttackIndex++;
	if (AttackIndex >= RightAttackMontages.Num())
		AttackIndex = 0;
}

void AProjectCKCharacter::PerformDodge()
{
	TargetRotateTimeline.Stop();
	SoftTarget = NULL;
	ChangeState(EPlayerStates::DODGE);
	DamageSystemComponent->IsInvincible = true;
	PlayAnimMontage(DodgeMontage);
}

void AProjectCKCharacter::SaveAttack()
{
	if (AttackInfo.AttackSaved)
	{
		AttackInfo.AttackSaved = false;
		if (CheckCurrentState({ EPlayerStates::ATTACKING }))
		{
			ChangeState(EPlayerStates::PASSIVE);
			Attack(AttackInfo.AttackType);
		}
		else
		{
			Attack(AttackInfo.AttackType);
		}
	}
}

void AProjectCKCharacter::SaveDodge()
{
	if (bSaveDodge)
	{
		bSaveDodge = false;
		if (CheckCurrentState({ EPlayerStates::DODGE }))
		{
			ChangeState(EPlayerStates::PASSIVE);
			PerformDodge();
		}
		else
		{
			PerformDodge();
		}
	}
}

void AProjectCKCharacter::SoftLock()
{
	if (TargetActor)
	{
		SoftTarget = NULL;
	}
	else
	{
		FVector StartPos = GetActorLocation();
		FVector EndPos = StartPos + GetCharacterMovement()->GetLastInputVector() * 500;
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypes.Add(Pawn);
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		FHitResult HitResult;
		
		bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPos, EndPos, 100, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);
		if (Result)
		{
			SoftTarget = HitResult.GetActor();
		}
	}
}

void AProjectCKCharacter::RotateToTarget()
{
	if (TargetActor || SoftTarget)
	{
		TargetRotateTimeline.PlayFromStart();
	}
}

void AProjectCKCharacter::RotateToTargetTimelineFunction(float Value)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor ? TargetActor->GetActorLocation() : SoftTarget->GetActorLocation());
	FRotator TargetRotation = FRotator(GetActorRotation().Pitch, LookAtRotation.Yaw, LookAtRotation.Roll);
	SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), TargetRotation, Value, false));
}

void AProjectCKCharacter::ResetState()
{
	ChangeState(EPlayerStates::PASSIVE);
	AttackIndex = 0;
	AttackInfo.AttackType = EAttackType::NONE;
	AttackInfo.AttackSaved = false;
	DamageSystemComponent->IsInvincible = false;
	bSaveDodge = false;
	TargetRotateTimeline.Stop();
	SoftTarget = NULL;

}

void AProjectCKCharacter::StartWeaponCollision()
{
	bActivateCollision = true;
	AlreadyHitActors.Empty();
}

void AProjectCKCharacter::EndWeaponCollision()
{
	bActivateCollision = false;
}

void AProjectCKCharacter::ChangeAttackTarget(AActor* NewTargetActor)
{
	auto NewTargetActorInterface = Cast<IDamagableInterface>(NewTargetActor);
	if(TargetActor)
		Cast<IDamagableInterface>(TargetActor)->Execute_UnsetAttackTarget(TargetActor);
	TargetActor = NewTargetActor;
	if(NewTargetActorInterface)
		NewTargetActorInterface->Execute_SetAttackTarget(NewTargetActor);
}

void AProjectCKCharacter::ChangeToContollerDesiredRotation()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AProjectCKCharacter::ChangeToRotationToMovement()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}
