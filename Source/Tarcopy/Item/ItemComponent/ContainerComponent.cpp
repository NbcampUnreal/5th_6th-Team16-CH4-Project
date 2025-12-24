#include "Item/ItemComponent/ContainerComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/ContainerData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Inventory/InventoryData.h"
#include "Item/ItemCommand/OpenContainerCommand.h"
#include "Net/UnrealNetwork.h"

void UContainerComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	if (HasAuthority() == false)
		return;

	if (Data == nullptr)
		return;

	InventoryData = NewObject<UInventoryData>(this);
	InventoryData->Init(Data->ContainerBound);
}

void UContainerComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	ensureMsgf(Data != nullptr, TEXT("No FoodData"));

	UOpenContainerCommand* OpenContainerCommand = NewObject<UOpenContainerCommand>(this);
	OpenContainerCommand->OwnerComponent = this;
	OpenContainerCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Open Inventory of {0}")), TextItemName);
	OpenContainerCommand->bExecutable = true;
	OutCommands.Add(OpenContainerCommand);
}

void UContainerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryData);
}

void UContainerComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::ContainerTable)->FindRow<FContainerData>(ItemData->ItemId, FString(""));
}
