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
