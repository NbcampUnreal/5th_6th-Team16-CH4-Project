#include "Item/ItemComponent/CraftComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/CraftSubsystem.h"
#include "Item/Data/ItemData.h"
#include "Item/Data/CraftData.h"
#include "Item/ItemComponent/ItemComponentInteractionData.h"
#include "Kismet/KismetSystemLibrary.h"

void UCraftComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UCraftComponent::GetInteractionDatas(TArray<struct FItemComponentInteractionData>& OutDatas)
{
	UCraftSubsystem* CraftSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCraftSubsystem>();
	if (IsValid(CraftSubsystem) == false)
		return;

	const FItemData* OwnerItemData = GetOwnerItemData();
	if (OwnerItemData == nullptr)
		return;

	FName ItemId = OwnerItemData->ItemId;
	const FCraftRecipe* Recipe = CraftSubsystem->GetCraftRecipe(ItemId);
	if (Recipe == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	const UDataTable* ItemTable = IsValid(DataTableSubsystem) == true ? DataTableSubsystem->GetTable(EDataTableType::ItemTable) : nullptr;
	if (IsValid(ItemTable) == false)
		return;

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

			FItemComponentInteractionData CraftInteractionData;
			CraftInteractionData.TextDisplay = FText::Format(FText::FromString(TEXT("Craft {0}")), FText::FromString(JoinedString));
			// CraftInteractionData.bIsInteractable
			// 인벤토리 및 근처에 재료 충분하고 도구 있는지 제작 가능한지 검사해서 true/false;
			CraftInteractionData.DelegateInteract.BindLambda(
				[&]()
				{
					ExecuteCraft(CraftDataHandle.RowName);
				});
			OutDatas.Add(CraftInteractionData);
		}
	}
}

void UCraftComponent::ExecuteCraft(const FName& CraftId)
{
	UKismetSystemLibrary::PrintString(GetWorld(), *CraftId.ToString());
}
