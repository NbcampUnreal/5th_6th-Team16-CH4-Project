// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_InventoryItem.h"

#include "UI/UW_Inventory.h"
#include "UI/InventoryDragDropOp.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/InventoryData.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"

FReply UUW_InventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UUW_InventoryItem::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UInventoryDragDropOp* Op = NewObject<UInventoryDragDropOp>(GetOwningPlayer());
	Op->ItemId = ItemId;
	Op->SourceInventory = SourceInventory;
	Op->SourceInventoryWidget = SourceInventoryWidget;
	Op->bRotated = bRotated;

	Op->GrabOffsetPx = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	UUW_InventoryItem* Proxy = CreateWidget<UUW_InventoryItem>(GetOwningPlayer(), GetClass());
	if (Proxy)
	{
		Proxy->InitItem(ItemId, SourceInventory, SourceInventoryWidget, bRotated);
		Proxy->ApplyProxyVisual();
		Proxy->SetRenderOpacity(0.4f);

		const FVector2D SizePx = GetItemPixelSize();

		USizeBox* Box = NewObject<USizeBox>(this);
		Op->DragBox = Box;
		Box->SetWidthOverride(SizePx.X);
		Box->SetHeightOverride(SizePx.Y);
		Box->AddChild(Proxy);

		Op->DefaultDragVisual = Box;
	}
	else
	{
		Op->DefaultDragVisual = this;
	}

	Op->Pivot = EDragPivot::MouseDown;

	OutOperation = Op;
}

void UUW_InventoryItem::InitItem(const FGuid& InItemId, UInventoryData* InSourceInventory, UUW_Inventory* InSourceWidget, bool bInRotated)
{
	ItemId = InItemId;
	SourceInventory = InSourceInventory;
	SourceInventoryWidget = InSourceWidget;
	bRotated = bInRotated;
}

void UUW_InventoryItem::ApplyProxyVisual()
{
	if (ItemBorder)
	{
		ItemBorder->SetBrushColor(FLinearColor(0, 0, 0, 0));
	}
	if (ItemBG)
	{
		ItemBG->SetBrushColor(FLinearColor(0, 0, 0, 0));
	}
}

FVector2D UUW_InventoryItem::GetItemPixelSize() const
{
	if (!IsValid(SourceInventory) || !IsValid(SourceInventoryWidget))
	{
		return FVector2D::ZeroVector;
	}

	const int32 CellPx = SourceInventoryWidget->GetCellSizePx();
	const FIntPoint SizeCells = SourceInventory->GetItemSizeByID(ItemId, bRotated);

	return FVector2D(SizeCells.X * CellPx, SizeCells.Y * CellPx);
}