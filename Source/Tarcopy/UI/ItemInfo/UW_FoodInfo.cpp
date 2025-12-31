// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/UW_FoodInfo.h"

#include "Item/ItemInstance.h"
#include "Item/ItemComponent/FoodComponent.h"
#include "Item/Data/FoodData.h"
#include "Components/TextBlock.h"

void UUW_FoodInfo::BindItem(UItemInstance* InItem)
{
	auto Data = InItem->GetItemComponent<UFoodComponent>()->GetData();

	HungerTxt->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), Data->Hunger)));

	ThirstTxt->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), Data->Thirst)));
}