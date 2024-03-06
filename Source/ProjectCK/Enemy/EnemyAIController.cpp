#include "EnemyAIController.h"
#include "NPCBaseCharacter.h"

#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include <Navigation/CrowdFollowingComponent.h>

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"

#include <ProjectCK/Interfaces/EnemyAI.h>


// Blackboard Keys
const FName AEnemyAIController::Key_AttackTarget(TEXT("AttackTarget"));
const FName AEnemyAIController::Key_State(TEXT("State"));
const FName AEnemyAIController::Key_InterestLocation(TEXT("InterestLocation"));
const FName AEnemyAIController::Key_AttackRadius(TEXT("AttackRadius"));
const FName AEnemyAIController::Key_DefendRadius(TEXT("DefendRadius"));

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
	AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("AISightConfig");
	AISightConfig->SightRadius = 800;
	AISightConfig->LoseSightRadius = 1200;
	AISightConfig->PeripheralVisionAngleDegrees = 60;
	AISightConfig->SetMaxAge(5);
	AISightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AISightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AIPerceptionComponent->ConfigureSense(*AISightConfig);

	AIHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>("AIHearingConfig");
	AIPerceptionComponent->ConfigureSense(*AIHearingConfig);

	AIDamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>("AIDamageConfig");
	AIPerceptionComponent->ConfigureSense(*AIDamageConfig);

	AIPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated_Delegate);
}


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ANPCBaseCharacter* const OwnerCharacter = Cast<ANPCBaseCharacter>(InPawn))
	{
		if (UBehaviorTree* const Tree = OwnerCharacter->GetBehaviorTree())
		{
			UBlackboardComponent* BlackboardComponent;
			UseBlackboard(Tree->GetBlackboardAsset(), BlackboardComponent);
			Blackboard = BlackboardComponent;
			RunBehaviorTree(Tree);

			auto OnwerCharacter = Cast<IEnemyAI>(InPawn);
			if (OnwerCharacter)
			{
				Blackboard->SetValueAsFloat(Key_AttackRadius, OnwerCharacter->Execute_GetAttackRadius(InPawn));
				Blackboard->SetValueAsFloat(Key_DefendRadius, OnwerCharacter->Execute_GetDefendRadius(InPawn));
			}
			FTimerHandle StartTimer;
			GetWorldTimerManager().SetTimer(StartTimer, this, &AEnemyAIController::RunAI, 0.2f, false);
		}
	}
}


void AEnemyAIController::RunAI()
{
	SetStatePassive();
}

void AEnemyAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent == nullptr) return;

	BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
}

void AEnemyAIController::SetStatePassive()
{
	Blackboard->SetValueAsEnum(Key_State, (uint8)EEnemyStates::PASSIVE);
}

void AEnemyAIController::SetStateAttacking(AActor* AttackTarget)
{
	Blackboard->SetValueAsEnum(Key_State, (uint8)EEnemyStates::ATTACKING);
	Blackboard->SetValueAsObject(Key_AttackTarget, AttackTarget);
	CurrentAttackTarget = AttackTarget;
}

void AEnemyAIController::SetStateInvestigating(FVector Location)
{
	Blackboard->SetValueAsEnum(Key_State, (uint8)EEnemyStates::INVESTIGATING);
	Blackboard->SetValueAsVector(Key_InterestLocation, Location);
}

EEnemyStates AEnemyAIController::GetCurrentState()
{
	return (EEnemyStates)Blackboard->GetValueAsEnum(Key_State);
}

void AEnemyAIController::OnPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus const Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		SensedSight(Actor);
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		SensedHearing(Actor);
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		SensedDamage(Actor);
	}
}

void AEnemyAIController::SensedSight(AActor* Actor)
{
	if (GetCurrentState() == EEnemyStates::PASSIVE && Actor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		SetStateAttacking(Actor);
	}
}

void AEnemyAIController::SensedHearing(AActor* Actor)
{
	if (GetCurrentState() == EEnemyStates::PASSIVE || GetCurrentState() == EEnemyStates::INVESTIGATING)
	{
		SetStateInvestigating(Actor->GetActorLocation());
	}
}

void AEnemyAIController::SensedDamage(AActor* Actor)
{
	if (GetCurrentState() == EEnemyStates::PASSIVE || GetCurrentState() == EEnemyStates::INVESTIGATING)
	{
		SetStateAttacking(Actor);
	}
}

