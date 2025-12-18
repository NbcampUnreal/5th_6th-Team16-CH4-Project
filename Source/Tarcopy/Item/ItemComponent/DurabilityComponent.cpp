#include "Item/ItemComponent/DurabilityComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/DurabilityData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemCommand/RepairCommand.h"

void UDurabilityComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::DurabilityTable)->FindRow<FDurabilityData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;

	Condition = Data->MaxCondition;
}

void UDurabilityComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
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

void UDurabilityComponent::LoseDurability(float Amount)
{
	Condition -= Amount;
	Condition = FMath::Max(Condition, 0.0f);
}

void UDurabilityComponent::RestoreDurability(float Amount)
{
	if (Data == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("No Durability data"));
		return;
	}

	Condition += Amount;
	Condition = FMath::Min(Condition, Data->MaxCondition);

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Condition = %f"), Condition));
}
