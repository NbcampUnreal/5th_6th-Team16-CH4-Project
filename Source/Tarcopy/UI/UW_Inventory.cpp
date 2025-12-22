// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_Inventory.h"

#include "Inventory/InventoryData.h"
#include "UI/UW_InventoryCell.h"
#include "UI/UW_InventoryItem.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Item/ItemInstance.h"
#include "UI/InventoryDragDropOp.h"

bool UUW_Inventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	ClearCellPreview();

	if (!BoundInventory || !InOperation)
	{
		return false;
	}

	UInventoryDragDropOp* Op = Cast<UInventoryDragDropOp>(InOperation);
	if (!Op || !IsValid(Op->SourceInventory))
	{
		return false;
	}

	const FVector2D LocalPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());

	const FVector2D TopLeftPx = LocalPos - Op->GrabOffsetPx;

	const int32 NewX = FMath::FloorToInt(TopLeftPx.X / CellSizePx);
	const int32 NewY = FMath::FloorToInt(TopLeftPx.Y / CellSizePx);
	const FIntPoint NewOrigin(NewX, NewY);

	const bool bOk = Op->SourceInventory->TryRelocateItem(Op->ItemId, BoundInventory, NewOrigin, Op->bRotated);
	if (!bOk)
	{
		return false;
	}

	RefreshItems();

	if (IsValid(Op->SourceInventoryWidget) && Op->SourceInventoryWidget.Get() != this)
	{
		Op->SourceInventoryWidget->RefreshItems();
	}

	return true;
}

bool UUW_Inventory::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	if (!BoundInventory || !InOperation)
	{
		ClearCellPreview();
		return false;
	}

	UInventoryDragDropOp* Op = Cast<UInventoryDragDropOp>(InOperation);
	if (!Op || !IsValid(Op->SourceInventory))
	{
		ClearCellPreview();
		return false;
	}

	Op->HoveredInventoryWidget = this;
	Op->LastScreenPos = InDragDropEvent.GetScreenSpacePosition();

	const FVector2D LocalPos = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	const FVector2D TopLeftPx = LocalPos - Op->GrabOffsetPx;

	const int32 NewX = FMath::FloorToInt(TopLeftPx.X / CellSizePx);
	const int32 NewY = FMath::FloorToInt(TopLeftPx.Y / CellSizePx);
	const FIntPoint Origin(NewX, NewY);

	ClearCellPreview();

	const FIntPoint ItemSize = Op->SourceInventory->GetItemSizeByID(Op->ItemId, Op->bRotated);
	if (ItemSize == FIntPoint::ZeroValue)
	{
		ClearCellPreview();
		return false;
	}

	const bool bCanPlace = BoundInventory->CanPlaceItemPreview(
		Op->ItemId,
		Op->SourceInventory,
		Origin,
		Op->bRotated
	);

	const FLinearColor Color = bCanPlace ? FLinearColor(0.f, 1.f, 0.f, 0.35f) : FLinearColor(1.f, 0.f, 0.f, 0.35f);

	ApplyCellPreview(Origin, ItemSize, Color);

	return true;
}

void UUW_Inventory::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	if (UInventoryDragDropOp* Op = Cast<UInventoryDragDropOp>(InOperation))
	{
		if (Op->HoveredInventoryWidget.Get() == this)
		{
			Op->HoveredInventoryWidget = nullptr;
		}
	}

	ClearCellPreview();
}

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

	Item->InitItem(NewItemID, BoundInventory, this, bRotated);
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

void UUW_Inventory::ForceUpdatePreviewFromOp(UInventoryDragDropOp* Op)
{
	if (!BoundInventory || !Op || !IsValid(Op->SourceInventory))
	{
		ClearCellPreview();
		return;
	}
	const FVector2D LocalPos = GetCachedGeometry().AbsoluteToLocal(Op->LastScreenPos);
	const FVector2D TopLeftPx = LocalPos - Op->GrabOffsetPx;

	const int32 NewX = FMath::FloorToInt(TopLeftPx.X / CellSizePx);
	const int32 NewY = FMath::FloorToInt(TopLeftPx.Y / CellSizePx);
	const FIntPoint Origin(NewX, NewY);

	ClearCellPreview();
	const FIntPoint ItemSize = Op->SourceInventory->GetItemSizeByID(Op->ItemId, Op->bRotated);
	if (ItemSize == FIntPoint::ZeroValue)
	{
		return;
	}

	const bool bCanPlace = BoundInventory->CanPlaceItemPreview(
		Op->ItemId,
		Op->SourceInventory,
		Origin,
		Op->bRotated
	);

	const FLinearColor Color = bCanPlace
		? FLinearColor(0.f, 1.f, 0.f, 0.35f)
		: FLinearColor(1.f, 0.f, 0.f, 0.35f);

	ApplyCellPreview(Origin, ItemSize, Color);
}

void UUW_Inventory::BuildGrid(FIntPoint GridSize)
{
	CachedGridSize = GridSize;
	CellWidgets.SetNum(GridSize.X * GridSize.Y);

	for (int32 Y = 0; Y < GridSize.Y; ++Y)
	{
		for (int32 X = 0; X < GridSize.X; ++X)
		{
			UUW_InventoryCell* Cell = CreateWidget<UUW_InventoryCell>(GetOwningPlayer(), CellWidgetClass);
			GridPanel->AddChildToUniformGrid(Cell, Y, X);

			CellWidgets[ToIndex(X, Y)] = Cell;
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

	CellWidgets.Empty();
	PreviewCells.Empty();
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

UUW_InventoryCell* UUW_Inventory::GetCell(int32 X, int32 Y) const
{
	if (X < 0 || Y < 0 || X >= CachedGridSize.X || Y >= CachedGridSize.Y)
	{
		return nullptr;
	}
	const int32 Idx = ToIndex(X, Y);
	return CellWidgets.IsValidIndex(Idx) ? CellWidgets[Idx].Get() : nullptr;
}

int32 UUW_Inventory::ToIndex(int32 X, int32 Y) const
{
	return Y * CachedGridSize.X + X;
}

void UUW_Inventory::ClearCellPreview()
{
	for (TObjectPtr<UUW_InventoryCell>& Cell : PreviewCells)
	{
		if (Cell)
		{
			Cell->ClearPreview();
		}
	}
	PreviewCells.Empty();
}

void UUW_Inventory::ApplyCellPreview(const FIntPoint& Origin, const FIntPoint& Size, const FLinearColor& Color)
{
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			UUW_InventoryCell* Cell = GetCell(Origin.X + X, Origin.Y + Y);
			if (Cell)
			{
				Cell->SetPreviewColor(Color);
				PreviewCells.Add(Cell);
			}
		}
	}
}