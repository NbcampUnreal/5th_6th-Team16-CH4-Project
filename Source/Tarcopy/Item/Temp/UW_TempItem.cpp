#include "Item/Temp/UW_TempItem.h"
#include "Item/Temp/UW_TempInteract.h"
#include "Components/TextBlock.h"

void UUW_TempItem::SetItem(int32 ItemId)
{
	TextItemId->SetText(FText::FromString(FString::FromInt(ItemId)));


}
