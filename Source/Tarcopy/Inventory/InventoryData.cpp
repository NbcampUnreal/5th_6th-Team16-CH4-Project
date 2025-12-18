// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryData.h"

#include "Item/ItemInstance.h"
#include "Misc/Guid.h"
#include "Item/Data/ItemData.h"

void UInventoryData::Init(const FIntPoint& InGridSize)
{
	InventoryID = FGuid::NewGuid();
	GridSize = InGridSize;
	Cells.SetNum(GridSize.X * GridSize.Y);

	for (FGuid& Cell : Cells)
	{
		Cell.Invalidate();
	}
}

UItemInstance* UInventoryData::FindItemByID(FGuid ItemID)
{
	if (TObjectPtr<UItemInstance>* Found = Items.Find(ItemID))
	{
		return Found->Get();
	}
	return nullptr;
}

FIntPoint UInventoryData::GetItemSizeByID(FGuid ItemID, bool bRotated)
{
	UItemInstance* Item = FindItemByID(ItemID);
	return GetItemSize(Item, bRotated);
}

bool UInventoryData::TryAddItem(UItemInstance* NewItem, const FIntPoint& Origin, bool bRotated)
{
	if (!IsValid(NewItem))
	{
		return false;
	}

	const FGuid ItemId = NewItem->GetInstanceId();
	if (Placements.Contains(ItemId))
	{
		return false;
	}

	if (!CheckCanPlace(NewItem, Origin, bRotated))
	{
		return false;
	}

	const FIntPoint Size = GetItemSize(NewItem, bRotated);
	FillCells(ItemId, Origin, Size);

	Placements.Add(ItemId, { Origin, bRotated });
	Items.Add(ItemId, NewItem);

	return true;
}

int32 UInventoryData::GetItemCountByItemId(FName InItemId) const
{
	int32 Count = 0;

	for (const TPair<FGuid, TObjectPtr<UItemInstance>>& Pair : Items)
	{
		const UItemInstance* Item = Pair.Value.Get();
		if (!IsValid(Item) || Item->GetData() == nullptr)
		{
			continue;
		}

		if (Item->GetData()->ItemId == InItemId)
		{
			++Count;
		}
	}

	return Count;
}

bool UInventoryData::TryConsumeItemsByItemId(FName InItemId, int32 Count)
{
	if (Count <= 0)
	{
		return true;
	}

	TArray<FGuid> Candidates;
	Candidates.Reserve(Count);

	// 제거 후보 수집
	for (const TPair<FGuid, TObjectPtr<UItemInstance>>& Pair : Items)
	{
		const UItemInstance* Item = Pair.Value.Get();
		if (!IsValid(Item) || Item->GetData() == nullptr)
		{
			continue;
		}

		if (Item->GetData()->ItemId == InItemId)
		{
			Candidates.Add(Pair.Key);
			if (Candidates.Num() >= Count)
			{
				break;
			}
		}
	}

	// 후보가 필요 갯수 이하면 false 반환하며 제거하지 않음
	if (Candidates.Num() < Count)
	{
		return false;
	}

	// 순회하며 제거
	for (const FGuid& InstanceId : Candidates)
	{
		RemoveItemByInstanceId(InstanceId);
	}

	return true;
}

bool UInventoryData::RemoveItemByInstanceId(const FGuid& InstanceId)
{
	if (!Placements.Contains(InstanceId))
	{
		return false;
	}

	ClearCells(InstanceId);

	Placements.Remove(InstanceId);
	Items.Remove(InstanceId);

	return true;
}

void UInventoryData::ClearAll()
{
	for (FGuid& Cell : Cells)
	{
		Cell.Invalidate();
	}

	Placements.Empty();
	Items.Empty();
}

bool UInventoryData::CheckCanPlace(const UItemInstance* InItem, const FIntPoint& Origin, bool bRotated) const
{
	const FIntPoint Size = GetItemSize(InItem, bRotated);

	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const int32 CellX = Origin.X + X;
			const int32 CellY = Origin.Y + Y;

			if (!IsInBounds(CellX, CellY))
			{
				return false;
			}

			const int32 Index = ToIndex(CellX, CellY);
			if (Cells[Index].IsValid())
			{
				return false;
			}
		}
	}
	return true;
}

FIntPoint UInventoryData::GetItemSize(const UItemInstance* InItem, bool bRotated) const
{
	FIntPoint Size = InItem->GetData()->InventoryBound;
	return bRotated ? FIntPoint(Size.Y, Size.X) : Size;
}

bool UInventoryData::IsInBounds(int32 X, int32 Y) const
{
	return X >= 0 && Y >= 0 && X < GridSize.X && Y < GridSize.Y;
}

int32 UInventoryData::ToIndex(int32 X, int32 Y) const
{
	return X + Y * GridSize.X;
}

void UInventoryData::FillCells(const FGuid& ItemId, const FIntPoint& Origin, const FIntPoint& Size)
{
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const int32 Index = ToIndex(Origin.X + X, Origin.Y + Y);
			Cells[Index] = ItemId;
		}
	}
}

void UInventoryData::ClearCells(const FGuid& ItemId)
{
	for (FGuid& Cell : Cells)
	{
		if (Cell == ItemId)
		{
			Cell.Invalidate();
		}
	}
}
