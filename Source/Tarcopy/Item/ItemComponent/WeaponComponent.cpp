#include "Item/ItemComponent/WeaponComponent.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemInstance.h"
#include "Item/ItemCommand/EquipCommand.h"

void UWeaponComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	UEquipCommand* EquipCommand = NewObject<UEquipCommand>(this);
	EquipCommand->TargetItem = GetOwnerItem();
	EquipCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Equip {0}")), TextItemName);
	// 인벤토리에서 공간 있는지 체크해야 함
	//EquipCommand->bExecutable = ;
	OutCommands.Add(EquipCommand);
}
