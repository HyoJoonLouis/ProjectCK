#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interfaces/EnemyAI.h"
#include "../Interfaces/DamagableInterface.h"
#include "NPCBaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackEndDelegate);

UCLASS()
class PROJECTCK_API ANPCBaseCharacter : public ACharacter, public IEnemyAI, public IDamagableInterface
{
	GENERATED_BODY()

public:
	ANPCBaseCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:	
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() const { return BTAsset; };

	// IEnemyAI
	FORCEINLINE virtual class APatrolSpline* GetPatrolRoute_Implementation() override;
	virtual float SetMovementSpeed_Implementation(EMovementSpeed Speed) override;
	virtual float GetAttackRadius_Implementation() override;
	virtual float GetDefendRadius_Implementation() override;
	virtual void Attack_Implementation() override;

	//IDamagableInterface
	FORCEINLINE virtual float GetCurrentHealth_Implementation() override;
	FORCEINLINE virtual float GetMaxHealth_Implementation() override;
	virtual void Heal_Implementation(float Amount) override;
	virtual bool TakeDamage_Implementation(FDamageInfo DamageInfo) override;
	FORCEINLINE virtual bool IsDead_Implementation() override;
	FORCEINLINE virtual bool IsAttacking_Implementation() override;

	virtual bool ReserveAttackToken_Implementation(int Amount) override;
	virtual void ReturnAttackToken_Implementation(int Amount) override;


	// Attack
	UFUNCTION()
	void OnAttackEnd(class UAnimMontage* AnimMontage, bool bInterrupted);

private:
	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BTAsset;
	UPROPERTY(EditAnywhere, Category = AI)
	class APatrolSpline* PatrolSpline;
	UPROPERTY(EditAnywhere, Category = Damage)
	class UDamageSystemComponent* DamageSystemComponent;
	
	// Attack
	UPROPERTY(EditAnywhere, Category = AI)
	class UAnimMontage* AttackMontage;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAttackEndDelegate AttackEndDelegate;

	UPROPERTY(EditAnywhere, Category = AI)
	class UAnimMontage* TakeDamageMontage;


protected:
	// IEnemyAI Properties
	UPROPERTY(EditAnywhere, Category = Movement)
	TMap<EMovementSpeed, float> MovementSpeedMap;
	UPROPERTY(EditAnywhere, Category = Distance)
	float AttackRadius;
	UPROPERTY(EditAnywhere, Category = Distance)
	float DefendRadius;
};
