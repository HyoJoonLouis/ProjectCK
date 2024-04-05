// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBaseCharacter.h"
#include "MeleeEnemy.generated.h"

UCLASS()
class PROJECTCK_API AMeleeEnemy : public ANPCBaseCharacter
{
	GENERATED_BODY()

public:
	AMeleeEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<class UNPCHealthBar> HealthBarSubclass;
	
	UPROPERTY()
	class UNPCHealthBar* HealthBar;

	virtual bool TakeDamage_Implementation(AActor* CauseActor, FDamageInfo DamageInfo) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UImpactSystemComponent* ImpactSystemComponent;
};
