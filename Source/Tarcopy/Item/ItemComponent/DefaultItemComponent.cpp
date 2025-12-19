#include "Item/ItemComponent/DefaultItemComponent.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemInstance.h"
#include "Item/ItemCommand/DropCommand.h"

void UDefaultItemComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UDefaultItemComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	UDropCommand* DropCommand = NewObject<UDropCommand>(this);
	DropCommand->OwnerItem = GetOwnerItem();
	DropCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Drop {0}")), TextItemName);
	DropCommand->bExecutable = true;
	OutCommands.Add(DropCommand);
}
