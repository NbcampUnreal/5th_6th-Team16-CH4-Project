// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipments/UW_EquipmentSlot.h"


#include "Components/SizeBox.h"

void UUW_EquipmentSlot::SetItem(UItemInstance*)
{
}

void UUW_EquipmentSlot::SetSize(FVector2D NewSize)
{
    SlotSize->SetWidthOverride(NewSize.X);
    SlotSize->SetHeightOverride(NewSize.Y);
}