#include "Item/ItemComponent/CraftComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/CraftSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/Data/CraftData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemCommand/CraftCommand.h"
#include "Character/MyCharacter.h"
#include "Inventory/InventoryData.h"
#include "Item/ItemInstance.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"

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
	if (IsValid(OwnerCharacter) == false)
		return;

	OwnerCharacter->GetNearbyInventoryDatas(InventoryDatas);
	
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

void UCraftComponent::ServereRPC_Craft_Implementation(const FName& CraftId)
{
	UWorld* World = GetWorld();
	if (IsValid(World) == false)
		return;

	UDataTableSubsystem* DataTableSubsystem = World->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	const UDataTable* CraftTable = IsValid(DataTableSubsystem) == true ? DataTableSubsystem->GetTable(EDataTableType::CraftTable) : nullptr;
	if (IsValid(CraftTable) == false)
		return;

	const FCraftData* CraftData = CraftTable->FindRow<FCraftData>(CraftId, FString(""));
	if (CraftData == nullptr)
		return;

	TArray<UInventoryData*> InventoryDatas;
	AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwnerCharacter());
	if (IsValid(OwnerCharacter) == false)
		return;
	
	OwnerCharacter->GetNearbyInventoryDatas(InventoryDatas);

	bool bCanCraft = true;
	TMap<FName, TArray<FItemSource>> ItemSourcesMap;
	for (const auto& Ingredient : CraftData->IngredientItems)
	{
		TArray<FItemSource>& ItemSources = ItemSourcesMap.FindOrAdd(Ingredient.Key);
		for (const auto& InventoryData : InventoryDatas)
		{
			if (IsValid(InventoryData) == false)
				continue;

			TArray<UItemInstance*> OutCandidates;
			InventoryData->GetItemCountByItemId(Ingredient.Key, OutCandidates);

			for (const auto& Item : OutCandidates)
			{
				ItemSources.Emplace(InventoryData, Item);

				if (ItemSources.Num() >= Ingredient.Value)
					break;
			}

			if (ItemSources.Num() >= Ingredient.Value)
				break;
		}

		if (ItemSources.Num() < Ingredient.Value)
		{
			bCanCraft = false;
			break;
		}
	}

	if (bCanCraft == false)
		return;

	for (const auto& Pair : ItemSourcesMap)
	{
		int32 NeedToRemove = CraftData->IngredientItems[Pair.Key];
		const TArray<FItemSource>& ItemSources = Pair.Value;
		for (int32 Idx = 0; Idx < NeedToRemove; ++Idx)
		{
			if (IsValid(ItemSources[Idx].OwnerInventory) == true && IsValid(ItemSources[Idx].ItemInstance) == true)
			{
				ItemSources[Idx].OwnerInventory->RemoveItem(ItemSources[Idx].ItemInstance);
			}
		}
	}

	for (const auto& GainedItem : CraftData->GainedItems)
	{
		for (int32 Count = 0; Count < GainedItem.Value; ++Count)
		{
			UItemInstance* ItemInstance = NewObject<UItemInstance>(this);
			ItemInstance->SetItemId(GainedItem.Key);

			FVector SpawnLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 40.0f;
			AItemWrapperActor* ItemWrapperActor = GetWorld()->SpawnActor<AItemWrapperActor>(
				AItemWrapperActor::StaticClass(),
				SpawnLocation,
				FRotator::ZeroRotator);

			if (IsValid(ItemWrapperActor) == true)
			{
				ItemWrapperActor->SetItemInstance(ItemInstance);
			}
		}
	}
}
