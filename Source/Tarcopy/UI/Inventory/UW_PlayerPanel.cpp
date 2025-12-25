// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/UW_PlayerPanel.h"

#include "Components/ScrollBox.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Inventory/PlayerInventoryComponent.h"
#include "Inventory/InventoryData.h"
#include "UI/UISubsystem.h"
#include "UI/Inventory/UW_InventoryBorder.h"
#include "UI/Inventory/UW_Inventory.h"

void UUW_PlayerPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_PlayerPanel::BindPlayerInventory(UPlayerInventoryComponent* InComp)
{
	BoundComp = InComp;

	BoundComp->OnInventoryReady.AddUObject(this, &UUW_PlayerPanel::HandleInventoryReady);

	RefreshInventories();
}

void UUW_PlayerPanel::RefreshInventories()
{
	if (!BoundComp || !InventoryScrollBox)
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP)
	{
		return;
	}

	UUISubsystem* UIS = LP->GetSubsystem<UUISubsystem>();
	if (!UIS)
	{
		return;
	}

	UInventoryData* Data = BoundComp->GetPlayerInventoryData();
	if (!Data)
	{
		return;
	}

	if (!CachedPlayerInvBorder)
	{
		CachedPlayerInvBorder = UIS->ShowInventoryUI(Data);
		if (!CachedPlayerInvBorder) return;

		InventoryScrollBox->ClearChildren();
		InventoryScrollBox->AddChild(CachedPlayerInvBorder);
	}
	else
	{
		Cast<UUW_Inventory>(CachedPlayerInvBorder->GetContentWidget())->BindInventory(Data);
	}
}

void UUW_PlayerPanel::HandleInventoryReady()
{
	RefreshInventories();
}
