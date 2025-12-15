// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Inventory.generated.h"

class UInventoryData;
class UUW_InventoryItem;
class UUW_InventoryCell;
class UUniformGridPanel;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_Inventory : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindInventory(UInventoryData* InData);

	void AddItemWidget(FGuid NewItemID, const FIntPoint& Origin, bool bRotated);

private:
	void BuildGrid(FIntPoint GridSize);
	void BuildItems();

	const int CellSizePx = 100;

	UPROPERTY()
	TObjectPtr<UInventoryData> BoundInventory;

	UPROPERTY()
	TMap<FGuid, UUW_InventoryItem*> ItemWidgets;


	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UUW_InventoryCell> CellWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UUW_InventoryItem> ItemWidgetClass;


	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UCanvasPanel> ItemCanvas;
};
