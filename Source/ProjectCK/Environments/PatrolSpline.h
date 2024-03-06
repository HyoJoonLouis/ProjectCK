#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolSpline.generated.h"

UCLASS()
class PROJECTCK_API APatrolSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolSpline();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void IncrementPatrolRoute();

	UFUNCTION(BlueprintCallable)
	FVector GetSplinePointWorldPosition();

private:
	class USplineComponent* PatrolSplineComponent;

	int PatrolIndex;
	int Direction;
};
