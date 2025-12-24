// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryData.generated.h"

class UItemInstance;

USTRUCT()
struct FItemPlacement
{
	GENERATED_BODY()

	UPROPERTY()
	FIntPoint Origin = {0, 0}; // 아이템 좌상단 좌표

	UPROPERTY()
	uint8 bRotated : 1 = false; // 회전 여부
};

/**
 * 
 */
UCLASS()
class TARCOPY_API UInventoryData : public UObject
{
	GENERATED_BODY()

public:
	void Init(const FIntPoint& InGridSize);

	FGuid GetID() const { return InventoryID; }

	FIntPoint GetGridSize() const { return GridSize; }

	const TMap<TObjectPtr<UItemInstance>, FItemPlacement>& GetPlacements() const { return Placements; }

	bool TryAddItem(UItemInstance* NewItem, const FIntPoint& Origin, bool bRotated);

	bool TryRelocateItem(UItemInstance* Item, UInventoryData* Dest, const FIntPoint& NewOrigin, bool bRotated);

	int32 GetItemCountByItemId(FName InItemId) const;

	bool TryConsumeItemsByItemId(FName InItemId, int32 Count);

	bool RemoveItem(UItemInstance* Item);

	void ClearAll();

	FIntPoint GetItemSize(const UItemInstance* InItem, bool bRotated) const;

	bool CanPlaceItemPreview(const UItemInstance* Item, const UInventoryData* Source, const FIntPoint& NewOrigin, bool bRotated) const;

private:
	bool CheckCanPlace(const UItemInstance* InItem, const FIntPoint& Origin, bool bRotated, const UItemInstance* IgnoreItem = nullptr) const;

	bool IsInBounds(int32 X, int32 Y) const;
	int32 ToIndex(int32 X, int32 Y) const;

	void FillCells(UItemInstance* Item, const FIntPoint& Origin, const FIntPoint& Size);
	void ClearCells(UItemInstance* Item);

	FGuid InventoryID;
	FIntPoint GridSize; // X = Width, Y = Height

	UPROPERTY()
	TArray<TWeakObjectPtr<UItemInstance>> Cells;

	UPROPERTY()
	TMap<TObjectPtr<UItemInstance>, FItemPlacement> Placements;

	UPROPERTY()
	TMap<FGuid, TObjectPtr<UItemInstance>> Items;
};
