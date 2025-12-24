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
	for (auto& Cell : Cells)
	{
		Cell = nullptr;
	}

	Placements.Empty();
}

bool UInventoryData::TryAddItem(UItemInstance* NewItem, const FIntPoint& Origin, bool bRotated)
{
	if (!IsValid(NewItem) || NewItem->GetData() == nullptr)
	{
		return false;
	}

	if (Placements.Contains(NewItem))
	{
		return false;
	}

	if (!CheckCanPlace(NewItem, Origin, bRotated))
	{
		return false;
	}

	const FIntPoint Size = GetItemSize(NewItem, bRotated);
	if (Size == FIntPoint::ZeroValue)
	{
		return false;
	}

	FillCells(NewItem, Origin, Size);
	Placements.Add(NewItem, { Origin, bRotated });

	return true;
}

bool UInventoryData::TryRelocateItem(UItemInstance* Item, UInventoryData* Dest, const FIntPoint& NewOrigin, bool bRotated)
{
	if (!IsValid(Dest) || !IsValid(Item) || Item->GetData() == nullptr)
	{
		return false;
	}

	FItemPlacement* Placement = Placements.Find(Item);
	if (!Placement)
	{
		return false;
	}

	if (Dest == this)
	{
		// 같은 인벤토리에서 이동 할 경우 체크에서 본인 제외
		if (!CheckCanPlace(Item, NewOrigin, bRotated, Item))
		{
			return false;
		}
	}
	else
	{
		if (!Dest->CheckCanPlace(Item, NewOrigin, bRotated))
		{
			return false;
		}
	}

	// 소스에서 제거
	ClearCells(Item);
	Placements.Remove(Item);

	// 목적지에 추가
	const FIntPoint Size = GetItemSize(Item, bRotated);
	if (Size == FIntPoint::ZeroValue)
	{
		return false;
	}

	Dest->FillCells(Item, NewOrigin, Size);
	Dest->Placements.Add(Item, { NewOrigin, bRotated });

	return true;
}


int32 UInventoryData::GetItemCountByItemId(FName InItemId) const
{
	int32 Count = 0;

	for (const TPair<TObjectPtr<UItemInstance>, FItemPlacement>& Pair : Placements)
	{
		const UItemInstance* Item = Pair.Key;
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

	TArray<TObjectPtr<UItemInstance>> Candidates;
	Candidates.Reserve(Count);

	// 제거 후보 수집
	for (const TPair<TObjectPtr<UItemInstance>, FItemPlacement>& Pair : Placements)
	{
		const UItemInstance* Item = Pair.Key;
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
	for (UItemInstance* Item : Candidates)
	{
		RemoveItem(Item);
	}

	return true;
}

bool UInventoryData::RemoveItem(UItemInstance* Item)
{
	if (!IsValid(Item))
	{
		return false;
	}

	if (!Placements.Contains(Item))
	{
		return false;
	}

	ClearCells(Item);
	Placements.Remove(Item);

	return true;
}

void UInventoryData::ClearAll()
{
	for (auto& Cell : Cells)
	{
		Cell = nullptr;
	}

	Placements.Empty();
}

FIntPoint UInventoryData::GetItemSize(const UItemInstance* InItem, bool bRotated) const
{
	if (!IsValid(InItem) || InItem->GetData() == nullptr)
	{
		return FIntPoint::ZeroValue;
	}

	FIntPoint Size = InItem->GetData()->InventoryBound;
	return bRotated ? FIntPoint(Size.Y, Size.X) : Size;
}

bool UInventoryData::CanPlaceItemPreview(const UItemInstance* Item, const UInventoryData* Source, const FIntPoint& NewOrigin, bool bRotated) const
{
	if (!IsValid(Source) || !IsValid(Item) || Item->GetData() == nullptr)
	{
		return false;
	}

	const UItemInstance* Ignore = (Source == this) ? Item : nullptr;
	return CheckCanPlace(Item, NewOrigin, bRotated, Ignore);
}

bool UInventoryData::CheckCanPlace(const UItemInstance* InItem, const FIntPoint& Origin, bool bRotated, const UItemInstance* IgnoreItem) const
{
	const FIntPoint Size = GetItemSize(InItem, bRotated);
	if (Size == FIntPoint::ZeroValue)
	{
		return false;
	}

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
				if (IgnoreItem && Cells[Index].Get() == IgnoreItem)
				{
					continue;
				}
				return false;
			}
		}
	}
	return true;
}

bool UInventoryData::IsInBounds(int32 X, int32 Y) const
{
	return X >= 0 && Y >= 0 && X < GridSize.X && Y < GridSize.Y;
}

int32 UInventoryData::ToIndex(int32 X, int32 Y) const
{
	return X + Y * GridSize.X;
}

void UInventoryData::FillCells(UItemInstance* Item, const FIntPoint& Origin, const FIntPoint& Size)
{
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			const int32 Index = ToIndex(Origin.X + X, Origin.Y + Y);
			Cells[Index] = Item;
		}
	}
}

void UInventoryData::ClearCells(UItemInstance* Item)
{
	for (auto& Cell : Cells)
	{
		if (Cell.Get() == Item)
		{
			Cell = nullptr;
		}
	}
}
