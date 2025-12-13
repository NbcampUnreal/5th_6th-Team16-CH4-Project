#include "Item/ItemComponent/FoodComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/FoodData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemComponent/ItemComponentInteractionData.h"
#include "Kismet/KismetSystemLibrary.h"

void UFoodComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MeleeWeaponTable)->FindRow<FFoodData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;

}

void UFoodComponent::GetInteractionDatas(TArray<struct FItemComponentInteractionData>& OutDatas)
{
	FText TextItemName = OwnerItem->GetData()->TextName;

	FItemComponentInteractionData IngestQuarterData;
	IngestQuarterData.TextDisplay = FText::Format(FText::FromString(TEXT("Ingest Quarter of {0}")), TextItemName);
	IngestQuarterData.bIsInteractable = Amount >= 1;
	IngestQuarterData.DelegateInteract.BindUObject(this, &ThisClass::IngestQuarter);
	OutDatas.Add(IngestQuarterData);

	FItemComponentInteractionData IngestHalfData;
	IngestHalfData.TextDisplay = FText::Format(FText::FromString(TEXT("Ingest Half of {0}")), TextItemName);
	IngestHalfData.bIsInteractable = Amount >= 2;
	IngestHalfData.DelegateInteract.BindUObject(this, &ThisClass::IngestHalf);
	OutDatas.Add(IngestHalfData);

	FItemComponentInteractionData IngestAllData;
	IngestAllData.TextDisplay = FText::Format(FText::FromString(TEXT("Ingest All of {0}")), TextItemName);
	IngestAllData.bIsInteractable = Amount >= 1;
	IngestAllData.DelegateInteract.BindUObject(this, &ThisClass::IngestAll);
	OutDatas.Add(IngestAllData);
}

void UFoodComponent::IngestQuarter()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("IngestQuarter"));
}

void UFoodComponent::IngestHalf()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("IngestHalf"));
}

void UFoodComponent::IngestAll()
{
	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("IngestAll"));
}
