// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectCKGameMode.h"
#include "ProjectCKCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectCKGameMode::AProjectCKGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
