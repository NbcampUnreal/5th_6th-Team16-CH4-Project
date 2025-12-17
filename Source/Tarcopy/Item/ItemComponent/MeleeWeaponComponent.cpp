#include "Item/ItemComponent/MeleeWeaponComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/MeleeWeaponData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"

void UMeleeWeaponComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MeleeWeaponTable)->FindRow<FMeleeWeaponData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;
}

void UMeleeWeaponComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	Super::GetCommands(OutCommands);
}

void UMeleeWeaponComponent::Attack()
{
}
