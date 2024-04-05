#include "MeleeEnemy.h"
#include "Components/WidgetComponent.h"
#include "../UI/NPCHealthBar.h"
#include "../Components/DamageSystemComponent.h"
#include "../Components/ImpactSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

AMeleeEnemy::AMeleeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetRelativeLocation(FVector(0, 0, 200));

	ImpactSystemComponent = CreateDefaultSubobject<UImpactSystemComponent>(TEXT("ImpactSystemComponent"));
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	WidgetComponent->SetWidgetClass(HealthBarSubclass);
	HealthBar = Cast<UNPCHealthBar>(WidgetComponent->GetWidget());

	HealthBar->SetHP(DamageSystemComponent->CurrentHealth / DamageSystemComponent->MaxHealth);

	HealthBar->AddHealthToVerticalBox(ImpactSystemComponent->Peek(0));
	HealthBar->AddHealthToVerticalBox(ImpactSystemComponent->Peek(1));
}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMeleeEnemy::TakeDamage_Implementation(AActor* CauseActor, FDamageInfo DamageInfo)
{
	if (CauseActor)
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CauseActor->GetActorLocation()));
	
	if (DamageInfo.AttackType == ImpactSystemComponent->Peek())
	{
		DamageInfo.Amount *= 2;
		ImpactSystemComponent->Pop();
		HealthBar->DeleteHealthToVerticalBox();
		HealthBar->AddHealthToVerticalBox(ImpactSystemComponent->Peek(1));
		GetMesh()->GetAnimInstance()->Montage_Play(TakeDamageMontage, 2.0f);
	}
	bool Result = DamageSystemComponent->TakeDamage(DamageInfo);
	if(Result)
	{
		HealthBar->SetHP(DamageSystemComponent->CurrentHealth / DamageSystemComponent->MaxHealth);
		return true;
	}
	return false;
}
