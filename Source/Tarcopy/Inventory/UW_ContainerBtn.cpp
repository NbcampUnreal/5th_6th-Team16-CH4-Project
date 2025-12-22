// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UW_ContainerBtn.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Inventory/WorldContainerComponent.h"

void UUW_ContainerBtn::NativeConstruct()
{
	Super::NativeConstruct();
	ContainerBtn->OnClicked.AddDynamic(this, &UUW_ContainerBtn::HandleClicked);
	RefreshVisual();
}

void UUW_ContainerBtn::BindContainer(UWorldContainerComponent* InContainer)
{
	Inventory = InInventory;
	DisplayName = InDisplayName;
	RefreshVisual();
}

void UUW_ContainerBtn::RefreshVisual()
{
	if (!ContainerName)
	{
		return;
	}

	if (!IsValid(Inventory))
	{
		ContainerName->SetText(FText::GetEmpty());
		return;
	}
	ContainerName->SetText(DisplayName);
}

void UUW_ContainerBtn::HandleClicked()
{
	if (!IsValid(Inventory))
	{
		return;
	}
	OnClickedWithInventory.Broadcast(Inventory);
}
