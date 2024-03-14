#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Response.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonClickedDelegate_OneParam, FText, TextInput);

UCLASS()
class PROJECTCK_API UResponse : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* TextBlock;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonClickedDelegate_OneParam ButtonClickedDelegate;

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetText(FText Text);

	UFUNCTION()
	void OnClickedButton();
};
