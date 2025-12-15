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

	BuildGrid(BoundInventory->GetGridSize());
	// BuildItems();
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

void UUW_Inventory::BuildItems()
{
}
