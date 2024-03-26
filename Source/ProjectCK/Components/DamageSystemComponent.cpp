// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystemComponent.h"
#include "../Structs/DamageInfo.h"

UDamageSystemComponent::UDamageSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UDamageSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void UDamageSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDamageSystemComponent::Heal(float Amount)
{
	if (IsDead)
		return;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
}

bool UDamageSystemComponent::TakeDamage(FDamageInfo DamageInfo)
{
	if (IsDead || (IsInvincible && !DamageInfo.ShouldDamageInvincible))
		return false;

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageInfo.Amount, 0, MaxHealth);
	
	if (CurrentHealth <= 0)
	{
		IsDead = true;
		if (OnDead.IsBound())
			OnDead.Broadcast();
		GetOwner()->Destroy();
	}
	else
	{
		if (OnDamaged.IsBound())
			OnDamaged.Broadcast(DamageInfo.DamageResponse);
	}
	
	return true;
}

bool UDamageSystemComponent::ReserveAttackToken(int Amount)
{
	if (AttackToken >= Amount)
	{
		AttackToken -= Amount;
		return true;
	}
	else
	{
		return false;
	}
}

void UDamageSystemComponent::ReturnAttackToken(int Amount)
{
	AttackToken += Amount;
}

