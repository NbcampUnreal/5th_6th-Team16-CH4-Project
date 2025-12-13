#include "Item/Temp/UW_TempItem.h"
#include "Item/Temp/UW_TempInteract.h"
#include "Components/TextBlock.h"
#include "Item/Data/ItemData.h"
#include "Components/PanelWidget.h"
#include "Item/ItemInstance.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "Item/ItemComponent/MeleeWeaponComponent.h"

void UUW_TempItem::SetItem(UItemInstance* Item)
{
	if (IsValid(Item) == false)
		return;

	TextItemId->SetText(Item->GetData()->TextName);

	//const UMeleeWeaponComponent* WeaponComponent = Item->GetItemComponent<UMeleeWeaponComponent>();
	const auto& Components = Item->GetItemComponents();
	TArray<struct FItemComponentInteractionData> InteractionDatas;
	for (const auto& Component : Components)
	{
		Component->GetInteractionDatas(InteractionDatas);
	}

	for (const auto& InteractionData : InteractionDatas)
	{
		UUW_TempInteract* NewInteractUI = CreateWidget<UUW_TempInteract>(this, InteractUIClass);
		if (IsValid(NewInteractUI) == false)
			continue;

		PanelInteract->AddChild(NewInteractUI);
		NewInteractUI->SetInteract(InteractionData);
		InteractUIs.Add(NewInteractUI);
	}

	/*

	if (IsValid(InteractUIClass) == false)
		return;

	const FItemData* ItemData = nullptr;
	auto& ItemRows = ItemTable->GetRowMap();
	for (const auto& ItemRow : ItemRows)
	{
		FItemData* Data = (FItemData*)ItemRow.Value;
		if (Data->ItemId == ItemId)
		{
			ItemData = Data;
			break;
		}
	}

	if (ItemData == nullptr)
		return;

	auto& InteractRows = InteractTable->GetRowMap();
	for (const auto& InteractRow : InteractRows)
	{
		FInteractionData* Data = (FInteractionData*)InteractRow.Value;
		if (Data->IngredientItems.Contains(ItemId) == true)
		{
			UUW_TempInteract* NewInteractUI = CreateWidget<UUW_TempInteract>(this, InteractUIClass);
			if (IsValid(NewInteractUI) == false)
				continue;

			PanelInteract->AddChild(NewInteractUI);
			NewInteractUI->SetInteract(*Data);
			InteractUIs.Add(NewInteractUI);
		}
	}*/
}
