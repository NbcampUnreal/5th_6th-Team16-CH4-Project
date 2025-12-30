#include "Item/ItemComponent/MedicalComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/MedicalData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemCommand/ItemNetworkCommand.h"
#include "Net/UnrealNetwork.h"
#include "Item/ItemNetworkContext.h"
#include "Common/HealthComponent.h"

void UMedicalComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UMedicalComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands, const FItemCommandContext& Context)
{
	Super::GetCommands(OutCommands, Context);

	if (Context.Instigator.IsValid() == false)
		return;

	UHealthComponent* HealthComponent = Context.Instigator->FindComponentByClass<UHealthComponent>();
	if (IsValid(HealthComponent) == false)
		return;

	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	ensureMsgf(Data != nullptr, TEXT("No MedicalData"));

	UItemNetworkCommand* RepairCommand = NewObject<UItemNetworkCommand>(this);
	FItemNetworkContext IngestAllActionContext;
	IngestAllActionContext.TargetItemComponent = this;
	IngestAllActionContext.ActionTag = TEXT("RestoreHealth");
	RepairCommand->ActionContext = IngestAllActionContext;
	RepairCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Restore HP {0}")), TextItemName);
	RepairCommand->bExecutable = !FMath::IsNearlyEqual(HealthComponent->GetMaxHP(), HealthComponent->GetCurrentHP());
	OutCommands.Add(RepairCommand);
}

void UMedicalComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MedicalTable)->FindRow<FMedicalData>(ItemData->ItemId, FString(""));
}

void UMedicalComponent::OnExecuteAction(AActor* InInstigator, const FItemNetworkContext& NetworkContext)
{
	Super::OnExecuteAction(InInstigator, NetworkContext);

	if (NetworkContext.ActionTag == TEXT("RestoreHealth"))
	{
		RestoreHealth(InInstigator);
	}
}

void UMedicalComponent::RestoreHealth(AActor* InInstigator)
{
	if (OwnerItem.IsValid() == false)
		return;

	if (Data == nullptr)
		return;

	if (IsValid(InInstigator) == false)
		return;

	UHealthComponent* HealthComponent = InInstigator->FindComponentByClass<UHealthComponent>();
	if (IsValid(HealthComponent) == false)
		return;

	HealthComponent->RestoreHP(Data->RestoreAmount);
	OwnerItem->RemoveFromSource();
}
