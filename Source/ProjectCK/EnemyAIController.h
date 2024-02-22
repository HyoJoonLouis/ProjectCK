// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnumTypes.h"
#include "EnemyAIController.generated.h"


UCLASS()
class PROJECTCK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController(const FObjectInitializer& ObjectInitializer);
	virtual void OnPossess(APawn* InPawn) override;

	void RunAI();
	void StopAI();

	// States
	UFUNCTION(BlueprintCallable)
	void SetStatePassive();
	UFUNCTION(BlueprintCallable)
	void SetStateAttacking(AActor* AttackTarget);
	UFUNCTION(BlueprintCallable)
	void SetStateInvestigating(FVector Location);

	EEnemyStates GetCurrentState();

	// Perception
	UFUNCTION()
	void OnPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus const Stimulus);
	void SensedSight(AActor* Actor);
	void SensedHearing(AActor* Actor);

public:
	// Blackboard Keys
	static const FName Key_AttackTarget;
	static const FName Key_State;
	static const FName Key_InterestLocation;
	static const FName Key_AttackRadius;
	static const FName Key_DefendRadius;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentAttackTarget;

private:
	// Perception
	UPROPERTY(EditAnywhere)
	class UAIPerceptionComponent* AIPerceptionComponent;
	class UAISenseConfig_Sight* AISightConfig;
	class UAISenseConfig_Hearing* AIHearingConfig;
};
