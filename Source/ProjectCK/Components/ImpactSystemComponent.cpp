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
	if (ImpactQueue.IsEmpty())
		return EAttackType::NONE;
	return ImpactQueue.Pop();	
}

EAttackType UImpactSystemComponent::Peek()
{
	if (ImpactQueue.IsEmpty())
		return EAttackType::NONE;
	return ImpactQueue[ImpactQueue.Num() - 1];
}

EAttackType UImpactSystemComponent::Peek(int index)
{
	if (ImpactQueue.Num() <= index)
		return EAttackType::NONE;
	return ImpactQueue[ImpactQueue.Num() - 1 - index];
}

