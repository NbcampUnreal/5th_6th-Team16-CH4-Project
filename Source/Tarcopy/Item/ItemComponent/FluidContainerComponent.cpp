#include "Item/ItemComponent/FluidContainerComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/FluidContainerData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

void UFluidContainerComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	if (Data == nullptr)
		return;

}

void UFluidContainerComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
}

void UFluidContainerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ContainedFluidId);
	DOREPLIFETIME(ThisClass, Amount);
}

void UFluidContainerComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::FluidContainerTable)->FindRow<FFluidContainerData>(ItemData->ItemId, FString(""));
}

void UFluidContainerComponent::ServerRPC_Fill_Implementation(float InAmount)
{
}

void UFluidContainerComponent::OnRep_PrintFluid()
{
	//UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s: %d left"), Amount));
}
