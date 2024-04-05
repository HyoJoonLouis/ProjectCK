// Fill out your copyright notice in the Description page of Project Settings.


#include "ImpactSystemComponent.h"

UImpactSystemComponent::UImpactSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UImpactSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UImpactSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

EAttackType UImpactSystemComponent::Pop()
{
	if (ImpactQueue.Num() == 0)
		return EAttackType::NONE;
	return ImpactQueue.Pop();	
}

EAttackType UImpactSystemComponent::Peek()
{
	if (ImpactQueue.Num() == 0)
		return EAttackType::NONE;
	return ImpactQueue[0];
}

EAttackType UImpactSystemComponent::Peek(int index)
{
	if (ImpactQueue.Num() <= index)
		return EAttackType::NONE;
	return ImpactQueue[index];
}

