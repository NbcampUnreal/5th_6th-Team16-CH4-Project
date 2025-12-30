// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemInfo/UW_DurabilityInfo.h"

#include "Item/ItemInstance.h"
#include "Item/ItemComponent/DurabilityComponent.h"
#include "Item/Data/DurabilityData.h"
#include "Components/TextBlock.h"

void UUW_DurabilityInfo::BindItem(UItemInstance* InItem)
{
	ChachedComponent = InItem->GetItemComponent<UDurabilityComponent>();
	auto Data = ChachedComponent->GetData();

	ConditionTxt->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), ChachedComponent->GetCondition(), Data->MaxCondition)));
}