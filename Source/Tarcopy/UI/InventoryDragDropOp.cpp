// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryDragDropOp.h"

#include "Inventory/PlayerInventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryData.h"
#include "UI/UW_Inventory.h"
#include "Inventory/LootScannerComponent.h"

void UInventoryDragDropOp::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	if (HoveredInventoryWidget.IsValid())
	{
		return;
	}

	if (!IsValid(SourceInventory))
	{
		return;
	}

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC)
	{
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return;
	}

	UPlayerInventoryComponent* InvComp = Pawn->FindComponentByClass<UPlayerInventoryComponent>();
	if (!InvComp)
	{
		return;
	}

	if (ULootScannerComponent* Scanner = Pawn->FindComponentByClass<ULootScannerComponent>())
	{
		if (SourceInventory == Scanner->GetGroundInventoryData())
		{
			if (SourceInventoryWidget)
			{
				SourceInventoryWidget->RefreshItems();
			}
			return;
		}
	}

	InvComp->RequestDropItemToWorld(SourceInventory, ItemId, bRotated);

	if (SourceInventoryWidget)
	{
		SourceInventoryWidget->RefreshItems();
	}
}
