// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipments/UW_EquipmentSlot.h"

#include "Item/ItemInstance.h"
#include "Components/SizeBox.h"
#include "UI/ItemInfo/UW_ItemInfo.h"

void UUW_EquipmentSlot::SetItem(UItemInstance* InItem)
{
    Item = InItem;
    SetItemInfo();
}

void UUW_EquipmentSlot::SetSize(FVector2D NewSize)
{
    SlotSize->SetWidthOverride(NewSize.X);
    SlotSize->SetHeightOverride(NewSize.Y);
}

void UUW_EquipmentSlot::SetItemInfo()
{
	if (!TooltipClass || !Item.IsValid())
	{
		return;
	}
	Tooltip = CreateWidget<UUW_ItemInfo>(GetOwningPlayer(), TooltipClass);
	Tooltip->BindItem(Item.Get());
	SetToolTip(Tooltip);
}