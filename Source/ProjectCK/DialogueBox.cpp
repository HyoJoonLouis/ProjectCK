#include "DialogueBox.h"
#include "Components/TextBlock.h"

void UDialogueBox::SetName(FText Text)
{
	NameBlock->Text = Text;
}

void UDialogueBox::SetDialogue(FText Text)
{
	TextBlock->Text = Text;
}
