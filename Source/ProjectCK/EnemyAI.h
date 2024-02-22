#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnumTypes.h"
#include "EnemyAI.generated.h"

UINTERFACE(MinimalAPI)
class UEnemyAI : public UInterface
{
	GENERATED_BODY()
};

class PROJECTCK_API IEnemyAI
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class APatrolSpline* GetPatrolRoute();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float SetMovementSpeed(EMovementSpeed Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetAttackRadius();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetDefendRadius();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Attack();
};
