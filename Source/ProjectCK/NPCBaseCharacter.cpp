#include "NPCBaseCharacter.h"
#include "EnemyAIController.h"
#include "PatrolSpline.h"
#include "DamageSystemComponent.h"
#include "DamageInfo.h"
#include "EnumTypes.h"
#include "GameFramework/CharacterMovementComponent.h"


ANPCBaseCharacter::ANPCBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	DamageSystemComponent = CreateDefaultSubobject<UDamageSystemComponent>(TEXT("DamageSystemComponent"));
	
}

void ANPCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ANPCBaseCharacter::OnAttackEnd);
}

APatrolSpline* ANPCBaseCharacter::GetPatrolRoute_Implementation()
{
	return PatrolSpline;
}

float ANPCBaseCharacter::SetMovementSpeed_Implementation(EMovementSpeed Speed)
{
	float speed;
	if (Speed == EMovementSpeed::IDLE)
		speed = 0;
	else if (Speed == EMovementSpeed::WALKING)
		speed = 100;
	else if (Speed == EMovementSpeed::JOGGING)
		speed = 300;
	else if (Speed == EMovementSpeed::SPRINTING)
		speed = 500;

	GetCharacterMovement()->MaxWalkSpeed = speed;

	return speed;

}

float ANPCBaseCharacter::GetAttackRadius_Implementation()
{
	return 150.0f;
}

float ANPCBaseCharacter::GetDefendRadius_Implementation()
{
	return 350.0f;
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

void ANPCBaseCharacter::Heal_Implementation(float Amount)
{
	DamageSystemComponent->Heal(Amount);
}

bool ANPCBaseCharacter::TakeDamage_Implementation(FDamageInfo DamageInfo)
{
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


void ANPCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
