#include "Response.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UResponse::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddDynamic(this, &UResponse::OnClickedButton);
}

void UResponse::SetText(FText Text)
{
	TextBlock->SetText(Text);
}

void UResponse::OnClickedButton()
{
	if (ButtonClickedDelegate.IsBound())
	{
		ButtonClickedDelegate.Broadcast(TextBlock->Text);
	}
}
