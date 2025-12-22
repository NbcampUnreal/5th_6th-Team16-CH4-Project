// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UW_ContainerBtn.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryData.h"

void UUW_ContainerBtn::NativeConstruct()
{
	Super::NativeConstruct();
	if (ContainerBtn)
	{
		ContainerBtn->OnClicked.AddDynamic(this, &UUW_ContainerBtn::HandleClicked);
	}
	RefreshVisual();
}

void UUW_ContainerBtn::BindInventory(UInventoryData* InInventory, const FText& InDisplayName)
{
	Container = InContainer;
	RefreshVisual();
}

void UUW_ContainerBtn::RefreshVisual()
{
	if (!ContainerName)
	{
		return;
	}

	if (!Container.IsValid())
	{
		ContainerName->SetText(FText::GetEmpty());
		return;
	}
	ContainerName->SetText(Container->GetDisplayName());
}

void UUW_ContainerBtn::HandleClicked()
{
	if (!Container.IsValid())
	{
		return;
	}
	OnClickedWithContainer.Broadcast(Container.Get());
}
