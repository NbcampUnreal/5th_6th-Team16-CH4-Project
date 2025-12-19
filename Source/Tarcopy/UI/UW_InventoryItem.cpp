// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UW_InventoryItem.h"

#include "UI/UW_Inventory.h"
#include "UI/InventoryDragDropOp.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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
	UInventoryDragDropOp* Op = NewObject<UInventoryDragDropOp>();
	Op->ItemId = ItemId;
	Op->SourceInventory = SourceInventory;
	Op->SourceInventoryWidget = SourceInventoryWidget;
	Op->bRotated = bRotated;

	Op->GrabOffsetPx = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	Op->DefaultDragVisual = this;
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
