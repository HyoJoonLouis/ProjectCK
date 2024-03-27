#include "NPCBaseCharacter.h"
#include "EnemyAIController.h"
#include "../Environments/PatrolSpline.h"
#include "../Components/DamageSystemComponent.h"
#include "../Structs/DamageInfo.h"
#include "../Structs/EnumTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


ANPCBaseCharacter::ANPCBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	DamageSystemComponent = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystemComponent"));

	MovementSpeedMap.Add(EMovementSpeed::IDLE, 0);
	MovementSpeedMap.Add(EMovementSpeed::WALKING, 100);
	MovementSpeedMap.Add(EMovementSpeed::JOGGING, 300);
	MovementSpeedMap.Add(EMovementSpeed::SPRINTING, 500);
	AttackRadius = 150;
	DefendRadius = 300;
}

void ANPCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ANPCBaseCharacter::OnAttackEnd);
	DamageSystemComponent->OnDead.AddDynamic(this, &ANPCBaseCharacter::OnDied);
}

APatrolSpline* ANPCBaseCharacter::GetPatrolRoute_Implementation()
{
	return PatrolSpline;
}

float ANPCBaseCharacter::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	float speed = MovementSpeedMap[Speed];

	GetCharacterMovement()->MaxWalkSpeed = speed;

	return speed;
}

float ANPCBaseCharacter::GetAttackRadius_Implementation()
{
	return AttackRadius;
}

float ANPCBaseCharacter::GetDefendRadius_Implementation()
{
	return DefendRadius;
}

bool ANPCBaseCharacter::ReserveAttackToken_Implementation(int Amount)
{
	return DamageSystemComponent->ReserveAttackToken(Amount);
}

void ANPCBaseCharacter::ReturnAttackToken_Implementation(int Amount)
{
	DamageSystemComponent->ReturnAttackToken(Amount);
}

void ANPCBaseCharacter::Attack_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
}

float ANPCBaseCharacter::GetCurrentHealth_Implementation()
{
	return DamageSystemComponent->CurrentHealth;
}

float ANPCBaseCharacter::GetMaxHealth_Implementation()
{
	return DamageSystemComponent->MaxHealth;
}

void ANPCBaseCharacter::SetAttackTarget_Implementation()
{
	if (IsValid(OverlayMaterialInstance))
	{
		GetMesh()->SetOverlayMaterial(OverlayMaterialInstance);
	}
}

void ANPCBaseCharacter::UnsetAttackTarget_Implementation()
{
	GetMesh()->SetOverlayMaterial(NULL);
}

void ANPCBaseCharacter::Heal_Implementation(float Amount)
{
	DamageSystemComponent->Heal(Amount);
}

bool ANPCBaseCharacter::TakeDamage_Implementation(AActor* CauseActor, FDamageInfo DamageInfo)
{
	if(CauseActor)
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CauseActor->GetActorLocation()));
	GetMesh()->GetAnimInstance()->Montage_Play(TakeDamageMontage, 2.0f);
	return DamageSystemComponent->TakeDamage(DamageInfo);
}

bool ANPCBaseCharacter::IsDead_Implementation()
{
	return DamageSystemComponent->IsDead;
}

bool ANPCBaseCharacter::IsAttacking_Implementation()
{
	return false;
}

void ANPCBaseCharacter::OnAttackEnd(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (AnimMontage == AttackMontage)
	{
		if (AttackEndDelegate.IsBound()) 
			AttackEndDelegate.Broadcast();
	}
}

void ANPCBaseCharacter::OnDied()
{
	Destroy();
}


void ANPCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
