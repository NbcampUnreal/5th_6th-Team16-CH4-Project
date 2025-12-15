#include "Item/CraftSubsystem.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/CraftData.h"

void UCraftSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCraftSubsystem::LoadRecipes()
{
	UDataTableSubsystem* DataTableSubsystem = IsValid(GetGameInstance()) == true ? GetGameInstance()->GetSubsystem<UDataTableSubsystem>() : nullptr;
	if (IsValid(DataTableSubsystem) == true)
	{
		const UDataTable* CraftTable = DataTableSubsystem->GetTable(EDataTableType::CraftTable);
		if (CraftTable == nullptr)
			return;

		auto& CraftRows = CraftTable->GetRowMap();
		for (const auto& Pair : CraftRows)
		{
			const FCraftData* CraftData = (FCraftData*)Pair.Value;
			if (CraftData == nullptr)
				continue;

			for (const auto& [IngredientId, IngredientCount] : CraftData->IngredientItems)
			{
				if (CraftRecipes.Contains(IngredientId) == false)
				{
					CraftRecipes.Add(IngredientId, FCraftRecipe());
				}

				FDataTableRowHandle NewRowHandle;
				NewRowHandle.DataTable = CraftTable;
				NewRowHandle.RowName = Pair.Key;
				CraftRecipes[IngredientId].CraftDataHandles.Add(NewRowHandle);
			}
		}
	}

	bIsLoaded = true;
}

const FCraftRecipe* UCraftSubsystem::GetCraftRecipe(const FName& IngredientId)
{
	if (bIsLoaded == false)
	{
		LoadRecipes();
	}
	return CraftRecipes.Find(IngredientId);
}
