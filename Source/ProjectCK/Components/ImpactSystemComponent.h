#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Structs/DamageInfo.h"
#include "Containers/Queue.h"
#include "ImpactSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTCK_API UImpactSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UImpactSystemComponent();
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION()
	EAttackType Pop();
	
	// If with UFUNCTION() Overloading is not possible
	EAttackType Peek();
	EAttackType Peek(int index);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EAttackType> ImpactQueue;
};
