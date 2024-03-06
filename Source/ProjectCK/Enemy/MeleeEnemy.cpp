#include "MeleeEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AMeleeEnemy::AMeleeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
