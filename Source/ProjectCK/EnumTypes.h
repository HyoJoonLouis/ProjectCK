// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMovementSpeed :uint8
{
	IDLE		UMETA(DisplayName = "Idle"),
	WALKING		UMETA(DisplayName = "Walking"),
	JOGGING		UMETA(DisplayName = "Jogging"),
	SPRINTING	UMETA(DisplayName = "Sprinting")
};

UENUM(BlueprintType)
enum class EEnemyStates : uint8
{
	PASSIVE		UMETA(DisplayName = "Passive"),
	ATTACKING	UMETA(DisplayName = "Attacking"),
	INVESTIGATING UMETA(DisplayName = "Investigating"),
	DEAD		UMETA(DisplayName = "Dead")
};