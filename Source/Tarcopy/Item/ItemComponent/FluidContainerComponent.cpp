#include "Item/ItemComponent/FluidContainerComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/FluidContainerData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"

void UFluidContainerComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::FluidContainerTable)->FindRow<FFluidContainerData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;

}

void UFluidContainerComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
}
