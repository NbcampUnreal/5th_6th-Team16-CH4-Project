#include "Item/ItemComponent/ClothingComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/ClothData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"

void UClothingComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MeleeWeaponTable)->FindRow<FClothData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;
}

void UClothingComponent::GetInteractionDatas(TArray<struct FItemComponentInteractionData>& OutDatas)
{
}
