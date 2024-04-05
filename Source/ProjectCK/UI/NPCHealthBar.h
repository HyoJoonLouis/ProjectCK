// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCK_API UNPCHealthBar : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* HealthVerticalBox;
	
	UPROPERTY(EditAnywhere, Category = Health)
	TSubclassOf<UUserWidget> LeftHealthSubclass;

	UPROPERTY(EditAnywhere, Category = Health)
	TSubclassOf<UUserWidget> RightHealthSubclass;

	TQueue<UUserWidget*> HealthQueue;

public:
	UFUNCTION()
	void SetHP(float value);

	UFUNCTION()
	bool AddHealthToVerticalBox(enum EAttackType Type);

	UFUNCTION()
	void DeleteHealthToVerticalBox();
};
