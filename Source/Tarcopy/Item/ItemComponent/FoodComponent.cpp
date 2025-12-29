#include "Item/ItemComponent/FoodComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/FoodData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemCommand/IngestCommand.h"
#include "Net/UnrealNetwork.h"
#include "Character/MyCharacter.h"
#include "Character/MoodleComponent.h"

const float UFoodComponent::TotalAmount = 4;

void UFoodComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UFoodComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands, const struct FItemCommandContext& Context)
{
	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	ensureMsgf(Data != nullptr, TEXT("No FoodData"));

	UIngestCommand* IngestQuarterCommand = NewObject<UIngestCommand>(this);
	IngestQuarterCommand->OwnerComponent = this;
	IngestQuarterCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Ingest Quarter of {0}")), TextItemName);
	IngestQuarterCommand->bExecutable = Amount >= 1;
	IngestQuarterCommand->EatAmount = 1;
	OutCommands.Add(IngestQuarterCommand);

	UIngestCommand* IngestHalfCommand = NewObject<UIngestCommand>(this);
	IngestHalfCommand->OwnerComponent = this;
	IngestHalfCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Ingest Half of {0}")), TextItemName);
	IngestHalfCommand->bExecutable = Amount >= 2;
	IngestHalfCommand->EatAmount = 2;
	OutCommands.Add(IngestHalfCommand);
	
	UIngestCommand* IngestAllCommand = NewObject<UIngestCommand>(this);
	IngestAllCommand->OwnerComponent = this;
	IngestAllCommand->TextDisplay = FText::Format(FText::FromString(TEXT("Ingest All of {0}")), TextItemName);
	IngestAllCommand->bExecutable = Amount >= 1;
	IngestAllCommand->EatAmount = Amount;
	OutCommands.Add(IngestAllCommand);
}

void UFoodComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Amount);
}

void UFoodComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::FoodTable)->FindRow<FFoodData>(ItemData->ItemId, FString(""));
}

void UFoodComponent::ServerRPC_Ingest_Implementation(int32 ConsumeAmount)
{
	if (ConsumeAmount > Amount)
		return;

	ACharacter* OwnerCharacter = GetOwnerCharacter();
	UMoodleComponent* Moodle = IsValid(OwnerCharacter) == true ? OwnerCharacter->FindComponentByClass<UMoodleComponent>() : nullptr;
	if (IsValid(Moodle) == false)
		return;

	Amount -= ConsumeAmount;
	OnRep_PrintAmount();

	float RestoreRatio = (float)ConsumeAmount / TotalAmount;
	Moodle->RestoreHunger(Data->Hunger * RestoreRatio);
	Moodle->RestoreThirst(Data->Thirst * RestoreRatio);
}

void UFoodComponent::OnRep_PrintAmount()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%d left"), Amount));
	if (OnUpdatedItemComponent.IsBound())
	{
		OnUpdatedItemComponent.Broadcast();
	}
}
