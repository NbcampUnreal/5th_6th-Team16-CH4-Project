#include "Item/Temp/UW_TempInteract.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Item/Data/InteractionData.h"

void UUW_TempInteract::SetInteract(const FInteractionData& InData)
{
	TextInteract->SetText(InData.TextInteract);
}
