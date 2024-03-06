#include "PatrolSpline.h"
#include "Components/SplineComponent.h"

APatrolSpline::APatrolSpline()
{
	PrimaryActorTick.bCanEverTick = false;

	PatrolSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	PatrolSplineComponent->SetupAttachment(RootComponent);
}

void APatrolSpline::BeginPlay()
{
	Super::BeginPlay();
}

void APatrolSpline::IncrementPatrolRoute()
{
	PatrolIndex = PatrolIndex + Direction;

	if (PatrolIndex == PatrolSplineComponent->GetNumberOfSplinePoints() - 1)
	{
		Direction = -1;
	}
	else if(PatrolIndex == 0)
	{
		Direction = 1;
	}
}

FVector APatrolSpline::GetSplinePointWorldPosition()
{
	return PatrolSplineComponent->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
}

void APatrolSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

