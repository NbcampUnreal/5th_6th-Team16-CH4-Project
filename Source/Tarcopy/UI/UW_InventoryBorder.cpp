// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_InventoryBorder.h"

#include "Components/NamedSlot.h"

void UUW_InventoryBorder::SetContentWidget(UWidget* InWidget)
{
	ContentSlot->SetContent(InWidget);
}

UWidget* UUW_InventoryBorder::GetContentWidget() const
{
	return ContentSlot->GetContent();
}
