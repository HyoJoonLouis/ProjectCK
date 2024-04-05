// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCHealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "../Structs/DamageInfo.h"

void UNPCHealthBar::SetHP(float value)
{
	HPProgressBar->SetPercent(value);
}

bool UNPCHealthBar::AddHealthToVerticalBox(enum EAttackType Type)
{
	UUserWidget* TempWidget;
	if (Type == EAttackType::LEFT)
		TempWidget = CreateWidget(GetWorld(), LeftHealthSubclass);
	else if (Type == EAttackType::RIGHT)
		TempWidget = CreateWidget(GetWorld(), RightHealthSubclass);
	else
		return false;

	HealthVerticalBox->AddChildToVerticalBox(TempWidget);
	HealthQueue.Enqueue(TempWidget);
	return true;
}

void UNPCHealthBar::DeleteHealthToVerticalBox()
{
	UUserWidget* TempWidget;
	HealthQueue.Dequeue(TempWidget);

	TempWidget->RemoveFromParent();
}
