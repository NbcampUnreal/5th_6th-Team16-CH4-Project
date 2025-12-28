#include "Item/ItemComponent/CraftComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/CraftSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/Data/CraftData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemCommand/CraftCommand.h"
#include "Character/MyCharacter.h"
#include "Inventory/InventoryData.h"

void UCraftComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UCraftComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	UCraftSubsystem* CraftSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCraftSubsystem>();
	if (IsValid(CraftSubsystem) == false)
		return;

	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));

	FName ItemId = OwnerItemData->ItemId;
	const FCraftRecipe* Recipe = CraftSubsystem->GetCraftRecipe(ItemId);
	if (Recipe == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	const UDataTable* ItemTable = IsValid(DataTableSubsystem) == true ? DataTableSubsystem->GetTable(EDataTableType::ItemTable) : nullptr;
	if (IsValid(ItemTable) == false)
		return;

	TArray<UInventoryData*> InventoryDatas;
	AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwnerCharacter());
	if (IsValid(OwnerCharacter) == true)
	{
		OwnerCharacter->GetNearbyInventoryDatas(InventoryDatas);
	}

	for (const auto& CraftDataHandle : Recipe->CraftDataHandles)
	{
		FCraftData* CraftData = CraftDataHandle.DataTable->FindRow<FCraftData>(CraftDataHandle.RowName, FString(""));
		if (CraftData != nullptr)
		{
			TArray<FString> GainedItemNames;
			for (const auto& GainedItem : CraftData->GainedItems)
			{
				const FItemData* GainedItemData = ItemTable->FindRow<FItemData>(GainedItem.Key, FString(""));
				if (GainedItemData != nullptr)
				{
					GainedItemNames.Add(GainedItemData->TextName.ToString());
				}
			}

			FString JoinedString = FString::Join(GainedItemNames, TEXT(", "));

			UCraftCommand* CraftCommand = NewObject<UCraftCommand>(this);
			CraftCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Craft {0}")), FText::FromString(JoinedString));

			CraftCommand->bExecutable = true;			
			for (const auto& Ingredient : CraftData->IngredientItems)
			{
				TArray<UItemInstance*> OutCandidates;
				for (const auto& InventoryData : InventoryDatas)
				{
					if (IsValid(InventoryData) == false)
						continue;

					InventoryData->GetItemCountByItemId(Ingredient.Key, OutCandidates);
				}

				if (OutCandidates.Num() < Ingredient.Value)
				{
					CraftCommand->bExecutable = false;
					break;
				}
			}
			CraftCommand->CraftTargetId = CraftDataHandle.RowName;
			OutCommands.Add(CraftCommand);
		}
	}
}
