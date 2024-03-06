// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Structs/DamageInfo.h"
#include "DamagableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamagableInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTCK_API IDamagableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentHealth();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxHealth();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool TakeDamage(FDamageInfo DamageInfo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsDead();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsAttacking();

	
	// Attack Token
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ReserveAttackToken(int Amount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ReturnAttackToken(int Amount);
};
