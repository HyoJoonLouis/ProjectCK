#include "ProjectCKCharacter.h"
#include "../ProjectCKGameInstance.h"
#include "../Components/DamageSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

	if (bActivateCollision)
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
		ObjectTypes.Add(Pawn);
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(GetOwner());
		TArray<FHitResult> HitResults;
		bool Result = UKismetSystemLibrary::SphereTraceMultiForObjects(this, Weapon->GetSocketLocation(FName("Start")), Weapon->GetSocketLocation(FName("End")), 30.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResults, true);
		
		if (Result)
		{
			for (const auto HitResult : HitResults)
			{
				if (AlreadyHitActors.Contains(HitResult.GetActor()))
					continue;

				AlreadyHitActors.AddUnique(HitResult.GetActor());
				auto HitDamageInterface = Cast<IDamagableInterface>(HitResult.GetActor());
				if (HitDamageInterface)
				{
					HitDamageInterface->Execute_TakeDamage(HitResult.GetActor(), this, CurrentDamageInfo);
				}
			}
		}

	}
	//// Change To FSM Later
	//if (isRightAttacking)
	//{
	//	APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	//	FVector StartPos = CameraManager->GetCameraLocation();
	//	FVector EndPos = StartPos + CameraManager->GetActorForwardVector() * 2000;
	//	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	//	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	//	ObjectTypes.Add(Pawn);
	//	TArray<AActor*> IgnoreActors;
	//	IgnoreActors.Add(GetOwner());
	//	FHitResult HitResult;

	//	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(this, StartPos, EndPos, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForDuration, HitResult, true);
	//	if (Result)
	//	{
	//		ChangeAttackTarget(HitResult.GetActor());
	//	}
	//	return;
	//}

	//if (CheckCurrentState({ EPlayerStates::PASSIVE }))
	//{
	//	FVector StartPos = GetActorLocation();
	//	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	//	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	//	ObjectTypes.Add(Pawn);
	//	TArray<AActor*> IgnoreActors;
	//	IgnoreActors.Add(GetOwner());
	//	FHitResult HitResult;

	//	bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPos, StartPos, 500.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true);

	//	if (Result)
	//	{
	//		ChangeState(EPlayerStates::ATTACKING);
	//	}
	//}


	//if (CheckCurrentState({ EPlayerStates::ATTACKING }))
	//{
	//	if (TargetActor && FVector::Distance(TargetActor->GetActorLocation(), GetActorLocation()) > 800)
	//	{
	//		ChangeAttackTarget(NULL);
	//		ChangeState(EPlayerStates::PASSIVE);
	//		return;
	//	}	

	//	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	//	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	//	ObjectTypes.Add(Pawn);
	//	TArray<AActor*> IgnoreActors;
	//	IgnoreActors.Add(GetOwner());
	//	FHitResult HitResult;

	//	FVector LastInputVector = GetCharacterMovement()->GetLastInputVector();
	//	if (LastInputVector.IsNearlyZero())
	//	{
	//		APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	//		LastInputVector = CameraManager->GetActorForwardVector();
	//	}
	//	FVector StartPos = GetActorLocation() + LastInputVector * 150;
	//	FVector EndPos = StartPos + LastInputVector * 500;
	//	bool Result = UKismetSystemLibrary::SphereTraceSingleForObjects(this, StartPos, EndPos, 150.0f, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::ForOneFrame, HitResult, true, FLinearColor::Black);

	//	if (Result)
	//	{
	//		auto HitActor = Cast<IDamagableInterface>(HitResult.GetActor());
	//		if (HitActor)
	//		{
	//			ChangeAttackTarget(HitResult.GetActor());
	//		}
	//	}
	//}
}

void AProjectCKCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectCKCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectCKCharacter::Look);
		
		EnhancedInputComponent->BindAction(LeftAttackAction, ETriggerEvent::Started, this, &AProjectCKCharacter::LeftMouse);
	
		EnhancedInputComponent->BindAction(RightAttackAction, ETriggerEvent::Started, this, &AProjectCKCharacter::RightMouse);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AProjectCKCharacter::Dodge);
		//EnhancedInputComponent->BindAction(RightAttackAction, ETriggerEvent::Completed, this, &AProjectCKCharacter::RightAttackEnd);

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
	//if (isAttacking)
	//	return;
	//if(LeftAttackMontages.Num() == 0)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Attack Montages!"));
	//if (!CheckCurrentState({ EPlayerStates::ATTACKING }))
	//	ChangeState(EPlayerStates::ATTACKING);

	//if (isRightAttacking)
	//{
	//	RightAttackEnd(NULL);
	//	if (TargetActor)
	//		SetActorLocation(TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal() * 100);
	//	return;
	//}

	//// If Target Actor is faraway
	//if (TargetActor && FVector::Distance(TargetActor->GetActorLocation(), GetActorLocation()) > 300)
	//{
	//	FMotionWarpingTarget Target = {};
	//	Target.Name = FName("AttackTarget");
	//	Target.Location = TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal() * 100;
	//	Target.Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation());
	//	MotionWarpingComponent->AddOrUpdateWarpTarget(Target);
	//	PlayAnimMontage(MotionWarpingMontage);
	//	AttackIndex = 0;
	//	//SetActorLocation(TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal() * 100);
	//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TargetActor->GetActorLocation() + (GetActorLocation() - TargetActor->GetActorLocation()).Normalize() * 100).ToString());
	//}
	//else
	//{
	//	if (!TargetActor)
	//		return;

	//	// if Target Actor is Near so that RootMotion is Not need
	//	if (FVector::Distance(TargetActor->GetActorLocation(), GetActorLocation()) < 150)
	//	{
	//		LeftAttackMontages[AttackIndex]->EnableRootMotionSettingFromMontage(false, ERootMotionRootLock::RefPose);
	//	}
	//	else
	//	{
	//		LeftAttackMontages[AttackIndex]->EnableRootMotionSettingFromMontage(true, ERootMotionRootLock::RefPose);
	//	}
	//	// Change It to Lerp or something else 
	//	// Should Use Motion Lerp?
	//	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetActor->GetActorLocation()));

	//	PlayAnimMontage(LeftAttackMontages[AttackIndex]);
	//	AttackIndex++;
	//	if (AttackIndex >= LeftAttackMontages.Num())
	//		AttackIndex = 0;
	//}
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
	if (CheckCurrentState({ EPlayerStates::DODGE }))
	{
		bSaveDodge = true;
	}
	else
	{
		PerformDodge();
	}
}

void AProjectCKCharacter::Attack(EAttackType AttackType)
{
	if (CheckCurrentState({ EPlayerStates::ATTACKING }))
		return;

	if (AttackType == EAttackType::LEFT)
		PerformLeftAttack(AttackIndex);
	if (AttackType == EAttackType::RIGHT)
		PerformRightAttack(AttackIndex);
}

void AProjectCKCharacter::PerformLeftAttack(int Index)
{
	ChangeState(EPlayerStates::ATTACKING);
	PlayAnimMontage(LeftAttackMontages[Index]);
	AttackIndex++;
	if (AttackIndex >= LeftAttackMontages.Num())
		AttackIndex = 0;
}



void AProjectCKCharacter::PerformRightAttack(int Index)
{
	ChangeState(EPlayerStates::ATTACKING);
	PlayAnimMontage(RightAttackMontages[Index]);
	AttackIndex++;
	if (AttackIndex >= RightAttackMontages.Num())
		AttackIndex = 0;
}

void AProjectCKCharacter::PerformDodge()
{
	ChangeState(EPlayerStates::DODGE);
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

void AProjectCKCharacter::ResetState()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Reset"));
	ChangeState(EPlayerStates::PASSIVE);
	AttackIndex = 0;
	AttackInfo.AttackType = EAttackType::NONE;
	AttackInfo.AttackSaved = false;
	bSaveDodge = false;
}

void AProjectCKCharacter::StartWeaponCollision()
{
	//isAttacking = true;
	bActivateCollision = true;
	AlreadyHitActors.Empty();
}

void AProjectCKCharacter::EndWeaponCollision()
{
	bActivateCollision = false;
	//isAttacking = false;
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
