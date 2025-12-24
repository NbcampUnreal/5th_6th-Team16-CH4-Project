// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/UW_Inventory.h"

#include "UW_InventoryItem.generated.h"

class UInventoryData;
class UUW_Inventory;
class UBorder;
class UItemInstance;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_InventoryItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

public:
	void InitItem(UItemInstance* InItem, UInventoryData* InSourceInventory, UUW_Inventory* InSourceWidget, bool bInRotated);

private:
	void ApplyProxyVisual();
	FVector2D GetItemPixelSize() const;

	void OpenCommandMenu(const FPointerEvent& InMouseEvent);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ItemBorder;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ItemBG;

	UPROPERTY()
	TWeakObjectPtr<UItemInstance> Item;

	UPROPERTY()
	TObjectPtr<UInventoryData> SourceInventory;

	UPROPERTY()
	TObjectPtr<UUW_Inventory> SourceInventoryWidget;

	UPROPERTY()
	bool bRotated = false;
};
