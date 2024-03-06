#pragma once

#include "CoreMinimal.h"
#include "NPCBaseCharacter.h"
#include "RangeEnemy.generated.h"

UCLASS()
class PROJECTCK_API ARangeEnemy : public ANPCBaseCharacter
{
	GENERATED_BODY()
	
public:
	ARangeEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
};
