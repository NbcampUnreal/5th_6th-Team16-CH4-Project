#include "Item/ItemComponent/DurabilityComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/DurabilityData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemCommand/RepairCommand.h"
#include "Net/UnrealNetwork.h"

void UDurabilityComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	if (Data == nullptr)
		return;

	if (HasAuthority() == false)
		return;

	Condition = Data->MaxCondition;
}

void UDurabilityComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands, const struct FItemCommandContext& Context)
{
	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	ensureMsgf(Data != nullptr, TEXT("No DurabilityData"));

	URepairCommand* RepairCommand = NewObject<URepairCommand>(this);
	RepairCommand->OwnerComponent = this;
	RepairCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Repair {0}")), TextItemName);
	RepairCommand->bExecutable = true;
	RepairCommand->Amount = 1.0f;
	OutCommands.Add(RepairCommand);
}

void UDurabilityComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Condition);
}

void UDurabilityComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::DurabilityTable)->FindRow<FDurabilityData>(ItemData->ItemId, FString(""));
}

void UDurabilityComponent::LoseDurability(float Amount)
{
	if (HasAuthority() == false)
		return;

	Condition -= Amount;
	Condition = FMath::Max(Condition, 0.0f);

	OnRep_PrintCondition();
}

void UDurabilityComponent::ServerRPC_RestoreDurability_Implementation(float Amount)
{
	if (HasAuthority() == false)
		return;

	if (Data == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("No Durability data"));
		return;
	}

	Condition += Amount;
	Condition = FMath::Min(Condition, Data->MaxCondition);
	
	OnRep_PrintCondition();
}

void UDurabilityComponent::OnRep_PrintCondition()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Condition = %f"), Condition));
	if (OnUpdatedItemComponent.IsBound())
	{
		OnUpdatedItemComponent.Broadcast();
	}
}
