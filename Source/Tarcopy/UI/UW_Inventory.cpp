// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_Inventory.h"

#include "Inventory/InventoryData.h"
#include "UI/UW_InventoryCell.h"
#include "UI/UW_InventoryItem.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Item/ItemInstance.h"

void UUW_Inventory::BindInventory(UInventoryData* InData)
{
	BoundInventory = InData;

	ClearGrid();
	BuildGrid(BoundInventory->GetGridSize());
	BuildItems();
}

void UUW_Inventory::AddItemWidget(FGuid NewItemID, const FIntPoint& Origin, bool bRotated)
{
	UUW_InventoryItem* Item = CreateWidget<UUW_InventoryItem>(GetOwningPlayer(), ItemWidgetClass);
	if (!Item)
	{
		return;
	}
	UCanvasPanelSlot* CanvasSlot = ItemCanvas->AddChildToCanvas(Item);
	CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
	CanvasSlot->SetAlignment(FVector2D(0.f, 0.f));

	FIntPoint ItemSize = BoundInventory->GetItemSizeByID(NewItemID, bRotated);

	const FVector2D PosPx(Origin.X * CellSizePx, Origin.Y * CellSizePx);
	const FVector2D SizePx(ItemSize.X * CellSizePx, ItemSize.Y * CellSizePx);

	CanvasSlot->SetPosition(PosPx);
	CanvasSlot->SetSize(SizePx);

	ItemWidgets.Add(NewItemID, Item);
}

void UUW_Inventory::RefreshItems()
{
	if (!BoundInventory)
	{
		return;
	}

	if (ItemCanvas)
	{
		ItemCanvas->ClearChildren();
	}
	ItemWidgets.Empty();

	BuildItems();
}

void UUW_Inventory::BuildGrid(FIntPoint GridSize)
{
	for (int32 Y = 0; Y < GridSize.Y; ++Y)
	{
		for (int32 X = 0; X < GridSize.X; ++X)
		{
			UUW_InventoryCell* Cell = CreateWidget<UUW_InventoryCell>(GetOwningPlayer(), CellWidgetClass);
			GridPanel->AddChildToUniformGrid(Cell, Y, X);
		}
	}
}

void UUW_Inventory::ClearGrid()
{
	if (GridPanel)
	{
		GridPanel->ClearChildren();
	}

	if (ItemCanvas)
	{
		ItemCanvas->ClearChildren();
	}

	ItemWidgets.Empty();
}

void UUW_Inventory::BuildItems()
{
	if (!BoundInventory || !ItemCanvas)
	{
		return;
	}

	ItemCanvas->ClearChildren();
	UE_LOG(LogTemp, Warning, TEXT("After Clear: CanvasChildren=%d"), ItemCanvas->GetChildrenCount());
	ItemWidgets.Empty();

	const TMap<FGuid, FItemPlacement>& Placements = BoundInventory->GetPlacements();
	for (const TPair<FGuid, FItemPlacement>& Pair : Placements)
	{
		const FGuid ItemInstanceId = Pair.Key;
		const FItemPlacement& Placement = Pair.Value;

		AddItemWidget(ItemInstanceId, Placement.Origin, Placement.bRotated);
		UE_LOG(LogTemp, Warning, TEXT("After Add: CanvasChildren=%d"), ItemCanvas->GetChildrenCount());
	}
}
