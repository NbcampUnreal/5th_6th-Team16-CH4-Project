#include "Item/EquipComponent.h"
#include "Item/Data/ItemData.h"

const float UEquipComponent::WeightMultiplier = 0.3f;

UEquipComponent::UEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	for (uint32 SlotBit = 1; SlotBit < (uint32)EEquipSlot::MAX_BASE; SlotBit <<= 1)
	{
		EquippedItems.Add((EEquipSlot)SlotBit, -1);
	}

	EquipItem(10000);
	EquipItem(10001);
	EquipItem(10002);
	EquipItem(10003);
	EquipItem(10004);
}

void UEquipComponent::EquipItem(int32 ItemId)
{
	const FItemData* Data = GetItemData(ItemId);
	checkf(Data != nullptr, TEXT("EquipComponent => There's no new item's data"));

	for (auto& Pair : EquippedItems)
	{
		if (Exclusive(Pair.Key, Data->EquipSlot) == true)
		{
			RemoveItem(Pair.Value);
			Pair.Value = ItemId;
		}
	}

	TotalWeight += Data->Weight * WeightMultiplier;
}

void UEquipComponent::RemoveItem(int32 ItemId)
{
	if (ItemId < 0)
		return;

	const FItemData* Data = GetItemData(ItemId);
	checkf(Data != nullptr, TEXT("EquipComponent => There's no equipped item's data"));

	for (auto& Pair : EquippedItems)
	{
		if (Pair.Value == ItemId)
		{
			Pair.Value = -1;
		}
	}

	TotalWeight -= Data->Weight * WeightMultiplier;
}

const FItemData* UEquipComponent::GetItemData(int32 InItemId) const
{
	if (ItemTable.IsValid() == false)
		return nullptr;

	const auto& Rows = ItemTable->GetRowMap();
	for (const auto& Row : Rows)
	{
		FItemData* Data = (FItemData*)Row.Value;
		if (Data->ItemId == InItemId)
		{
			return Data;
		}
	}

	return nullptr;
}
