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

public:
};
