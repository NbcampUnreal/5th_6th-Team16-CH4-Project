#include "Item/ItemComponent/ClothingComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/ClothData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"

void UClothingComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UClothingComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	ensureMsgf(Data != nullptr, TEXT("No ClothData"));

	const UEnum* ENumTypeBL = StaticEnum<EBodyLocation>();
	FText TextBodyLocation = ENumTypeBL->GetDisplayNameTextByValue((int64)Data->BodyLocation);

	/*FItemComponentInteractionData EquipClothData;
	EquipClothData.TextDisplay = FText::Format(FText::FromString(TEXT("Equip {0} on {1}")), TextItemName, TextBodyLocation);
	EquipClothData.bIsInteractable = true;
	EquipClothData.DelegateInteract.BindUObject(this, &ThisClass::EquipCloth);
	OutDatas.Add(EquipClothData);*/
}

void UClothingComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::ClothTable)->FindRow<FClothData>(ItemData->ItemId, FString(""));
}
