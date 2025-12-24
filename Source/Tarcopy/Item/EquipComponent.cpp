#include "Item/EquipComponent.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemInstance.h"
#include "Item/ItemComponent/WeaponComponent.h"
#include "GameFramework/Character.h"
#include "Item/DataTableSubsystem.h"

const float UEquipComponent::WeightMultiplier = 0.3f;

UEquipComponent::UEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	for (uint32 SlotBit = 1; SlotBit < (uint32)EBodyLocation::MAX_BASE; SlotBit <<= 1)
	{
		EquippedItems.Add((EBodyLocation)SlotBit, nullptr);
	}

	// 인벤토리 없어서 임시 테스트용으로 부위 아무데나 정해서 Equip에 넣고 캐릭터에서 Equip에 장착된 아이템 표시되게 해서 테스트 중
	UItemInstance* NewItem = NewObject<UItemInstance>(this);
	NewItem->SetData(GetItemData(TEXT("Axe1")));
	EquipItem(EBodyLocation::RightHand, NewItem);
}

UItemInstance* UEquipComponent::GetEquippedItem(EBodyLocation Bodylocation) const
{
	auto* EquippedItemPtr = EquippedItems.Find(Bodylocation);
	return EquippedItemPtr != nullptr ? *EquippedItemPtr : nullptr;
}

void UEquipComponent::EquipItem(EBodyLocation BodyLocation, UItemInstance* Item)
{
	if (IsValid(Item) == false)
		return;

	const FItemData* ItemData = Item->GetData();
	if (ItemData == nullptr)
		return;

	for (auto& Pair : EquippedItems)
	{
		if (Exclusive(Pair.Key, BodyLocation) == true)
		{
			RemoveItem(Pair.Value);
			Pair.Value = Item;
		}
	}

	TotalWeight += ItemData->Weight * WeightMultiplier;
}

void UEquipComponent::RemoveItem(UItemInstance* Item)
{
	if (IsValid(Item) == false)
		return;

	const FItemData* ItemData = Item->GetData();
	checkf(ItemData != nullptr, TEXT("EquipComponent => There's no equipped item's data"));

	for (auto& Pair : EquippedItems)
	{
		if (IsValid(Pair.Value) == true)
		{
			const FItemData* CompareData = Pair.Value->GetData();
			if (CompareData != nullptr && CompareData->ItemId != ItemData->ItemId)
				continue;
		}

		// Equipment에 이상한 값 들어있거나 유효하지 않은 상태면 정리
		// 장착한 아이템이 지울 아이템이면 정리
		Pair.Value = nullptr;
	}

	TotalWeight -= ItemData->Weight * WeightMultiplier;
}

void UEquipComponent::ExecuteAttack()
{
	UItemInstance* ItemOnHand = GetEquippedItem(EBodyLocation::RightHand);
	if (IsValid(ItemOnHand) == false)
	{
		ItemOnHand = GetEquippedItem(EBodyLocation::LeftHand);
	}

	if (IsValid(ItemOnHand) == false)
		return;

	UWeaponComponent* WeaponComponent = ItemOnHand->GetItemComponent<UWeaponComponent>();
	if (IsValid(WeaponComponent) == false)
		return;

	WeaponComponent->ExecuteAttack(Cast<ACharacter>(GetOwner()));
}

void UEquipComponent::CancelActions()
{
	UItemInstance* ItemOnHand = GetEquippedItem(EBodyLocation::RightHand);
	if (IsValid(ItemOnHand) == false)
	{
		ItemOnHand = GetEquippedItem(EBodyLocation::LeftHand);
	}

	if (IsValid(ItemOnHand) == false)
		return;

	ItemOnHand->CancelAllComponentActions();
}

const FItemData* UEquipComponent::GetItemData(const FName& InItemId) const
{
	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return nullptr;

	const UDataTable* ItemTable = DataTableSubsystem->GetTable(EDataTableType::ItemTable);
	if (IsValid(ItemTable) == false)
		return nullptr;

	return ItemTable->FindRow<FItemData>(InItemId, FString(""));
}
