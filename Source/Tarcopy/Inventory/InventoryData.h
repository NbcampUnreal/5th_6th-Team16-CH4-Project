// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryData.generated.h"

class UItemInstance;
struct FGuid;

USTRUCT()
struct FItemPlacement
{
	GENERATED_BODY()

	UPROPERTY()
	FIntPoint Origin; // 아이템 좌상단 좌표

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
	UItemInstance* FindItemByID(FGuid ItemID);
	FIntPoint GetItemSizeByID(FGuid ItemID, bool bRotated);
	const TMap<FGuid, FItemPlacement>& GetPlacements() const { return Placements; }

	bool TryAddItem(UItemInstance* NewItem, const FIntPoint& Origin, bool bRotated);

	bool TryRelocateItem(const FGuid& ItemId, UInventoryData* Dest, const FIntPoint& NewOrigin, bool bRotated);

	int32 GetItemCountByItemId(FName InItemId) const;

	bool TryConsumeItemsByItemId(FName InItemId, int32 Count);

	bool RemoveItemByInstanceId(const FGuid& InstanceId);

	void ClearAll();

private:
	bool CheckCanPlace(const UItemInstance* InItem, const FIntPoint& Origin, bool bRotated, const FGuid* IgnoreId = nullptr) const;

	FIntPoint GetItemSize(const UItemInstance* InItem, bool bRotated) const;
	bool IsInBounds(int32 X, int32 Y) const;
	int32 ToIndex(int32 X, int32 Y) const;

	void FillCells(const FGuid& ItemId, const FIntPoint& Origin, const FIntPoint& Size);
	void ClearCells(const FGuid& ItemId);

	FGuid InventoryID;
	FIntPoint GridSize; // X = Width, Y = Height

	UPROPERTY()
	TArray<FGuid> Cells;

	UPROPERTY()
	TMap<FGuid, FItemPlacement> Placements;

	UPROPERTY()
	TMap<FGuid, TObjectPtr<UItemInstance>> Items;
};
