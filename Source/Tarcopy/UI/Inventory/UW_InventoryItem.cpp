// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Inventory/UW_InventoryItem.h"

#include "UI/Inventory/UW_Inventory.h"
#include "UI/Inventory/InventoryDragDropOp.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/InventoryData.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "UI/Inventory/UW_ItemCommandMenu.h"
#include "Item/ItemInstance.h"
#include "UI/UISubsystem.h"
#include "Blueprint/WidgetLayoutLibrary.h"

FReply UUW_InventoryItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OpenCommandMenu(InMouseEvent);
		return FReply::Handled();
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

void UUW_InventoryItem::OpenCommandMenu(const FPointerEvent& InMouseEvent)
{
	if (!IsValid(SourceInventory))
	{
		return;
	}

	UItemInstance* ItemInst = SourceInventory->FindItemByID(ItemId);
	if (!IsValid(ItemInst))
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

	const FVector2D ViewportPos = UWidgetLayoutLibrary::GetMousePositionOnViewport(PC);
	UIS->ShowItemCommandMenu(ItemInst, ViewportPos);
}
