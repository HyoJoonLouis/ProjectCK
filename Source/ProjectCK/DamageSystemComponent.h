// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageInfo.h"
#include "DamageSystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageDelegate, EDamageResponse, DamageResponse);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTCK_API UDamageSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDamageSystemComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Heal(float Amount);
	UFUNCTION(BlueprintCallable)
	bool TakeDamage(FDamageInfo DamageInfo);

	UFUNCTION(BlueprintCallable)
	bool ReserveAttackToken(int Amount);
	UFUNCTION(BlueprintCallable)
	void ReturnAttackToken(int Amount);

public:
	UPROPERTY(EditAnywhere)
	float MaxHealth;
	UPROPERTY(VisibleAnywhere)
	float CurrentHealth;

	UPROPERTY(EditAnywhere)
	bool IsInvincible;
	UPROPERTY(EditAnywhere)
	bool IsDead;
	
	UPROPERTY(BlueprintAssignable)
	FOnDeadDelegate OnDead;
	UPROPERTY(BlueprintAssignable)
	FOnDamageDelegate OnDamaged;

	UPROPERTY(EditAnywhere)
	int AttackToken;
	
};
