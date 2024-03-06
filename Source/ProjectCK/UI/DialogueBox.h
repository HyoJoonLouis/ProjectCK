// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCK_API UDialogueBox : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameBlock;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextBlock;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UOverlay* ResponseListPanel;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetName(FText Text);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetDialogue(FText Text);	
};
