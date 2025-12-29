#include "Item/EquipComponent.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemInstance.h"
#include "Item/ItemComponent/WeaponComponent.h"
#include "GameFramework/Character.h"
#include "Item/DataTableSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"
#include "Character/MyCharacter.h"
#include "Inventory/InventoryData.h"
#include "Item/ItemComponent/ClothingComponent.h"
#include "Item/Data/ClothDefensePreset.h"

const float UEquipComponent::WeightMultiplier = 0.3f;

UEquipComponent::UEquipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UEquipComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (IsValid(Owner) == false || Owner->HasAuthority() == false)
		return;

	for (uint32 SlotBit = 1; SlotBit < (uint32)EBodyLocation::MAX_BASE; SlotBit <<= 1)
	{
		EquippedItemInfos.Emplace((EBodyLocation)SlotBit, nullptr);
	}

	// 인벤토리 없어서 임시 테스트용으로 부위 아무데나 정해서 Equip에 넣고 캐릭터에서 Equip에 장착된 아이템 표시되게 해서 테스트 중
	UItemInstance* NewItem = NewObject<UItemInstance>(this);
	NewItem->SetItemId(TestEquippedItem);
	EquipItem(EBodyLocation::RightHand, NewItem);

	TArray<FName> Clothes = { FName(TEXT("TShirts0")), FName(TEXT("Shirt0")), FName(TEXT("Sweater0")), FName(TEXT("Bottoms0")), FName(TEXT("Back0")) };
	for (const auto& Cloth : Clothes)
	{
		NewItem = NewObject<UItemInstance>(this);
		NewItem->SetItemId(Cloth);
		EBodyLocation BodyLocation = NewItem->GetItemComponent<UClothingComponent>()->GetData()->BodyLocation;
		EquipItem(BodyLocation, NewItem);
	}
}

void UEquipComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquippedItemInfos);
}

bool UEquipComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto& EquippedItemInfo : EquippedItemInfos)
	{
		UItemInstance* Item = EquippedItemInfo.Item;
		if (IsValid(Item) == true)
		{
			bWroteSomething |= Item->ReplicateSubobjects(Channel, Bunch, RepFlags);
		}
	}
	return bWroteSomething;
}

UItemInstance* UEquipComponent::GetEquippedItem(EBodyLocation Bodylocation) const
{
	auto* EquippedItemPtr = EquippedItemInfos.FindByPredicate([Bodylocation](const FEquippedItemInfo ItemInfo)->bool { return ItemInfo.Location == Bodylocation; });
	return EquippedItemPtr != nullptr ? EquippedItemPtr->Item : nullptr;
}

void UEquipComponent::ServerRPC_EquipItem_Implementation(EBodyLocation BodyLocation, UItemInstance* Item)
{
	// 임시 나중에 Inventory에서 서버작업 해주면 거기서 해야 함
	AMyCharacter* Character = Cast<AMyCharacter>(GetOwner());
	if (IsValid(Character) == false)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("No Owner MyCharacter"));
		return;
	}

	TArray<UInventoryData*> InventoryDatas;
	Character->GetNearbyInventoryDatas(InventoryDatas);
	bool bIsExist = false;
	for (const auto& InventoryData : InventoryDatas)
	{
		if (InventoryData->RemoveItem(Item) == true)
		{
			bIsExist = true;
			break;
		}
	}

	UKismetSystemLibrary::PrintString(GetWorld(), bIsExist ? TEXT("bIsExist = true") : TEXT("bIsExist = false"));
	if (bIsExist == false)
		return;
	// 여기까지

	EquipItem(BodyLocation, Item);
}

void UEquipComponent::EquipItem(EBodyLocation BodyLocation, UItemInstance* Item)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (IsValid(OwnerCharacter) == false || OwnerCharacter->HasAuthority() == false)
		return;

	if (OwnerCharacter->HasAuthority() == false)
		return;

	if (IsValid(Item) == false)
		return;

	Item->SetOwnerObject(this);
	Item->SetOwnerCharacter(OwnerCharacter);

	const FItemData* ItemData = Item->GetData();
	if (ItemData == nullptr)
		return;

	UKismetSystemLibrary::PrintString(GetWorld(), *ItemData->ItemId.ToString());
	for (auto& EquippedItemInfo : EquippedItemInfos)
	{
		if (Exclusive(EquippedItemInfo.Location, BodyLocation) == true)
		{
			UnequipItem(EquippedItemInfo.Item);
			EquippedItemInfo.Item = Item;
		}
	}

	TotalWeight += ItemData->Weight * WeightMultiplier;

	UClothingComponent* ClothComponent = Item->GetItemComponent<UClothingComponent>();
	if (IsValid(ClothComponent) == true)
	{
		const FClothData* ClothData = ClothComponent->GetData();
		if (IsValid(ClothData->DefensePreset) == true)
		{
			FItemDamageReduce NewItemDamageReduce;
			for (const auto& PhysMat : ClothData->DefensePreset->DefenseMats)
			{
				NewItemDamageReduce.DamageReduces.Emplace(PhysMat, ClothData->DamageReduce);
			}
			ItemDamageReduces.Add(Item, NewItemDamageReduce);
			CalculateFinalDamageTakenMultiplier();
		}
	}

	/*UKismetSystemLibrary::PrintString(GetWorld(),
		IsValid(Item->GetOwnerCharacter()) == true ?
		*Item->GetOwnerCharacter()->GetName() : TEXT("Item No Owner"));*/
}

void UEquipComponent::ServerRPC_UnequipItem_Implementation(UItemInstance* Item)
{
	UnequipItem(Item);
}

void UEquipComponent::UnequipItem(UItemInstance* Item)
{
	AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwner());
	if (IsValid(OwnerCharacter) == false || OwnerCharacter->HasAuthority() == false)
		return;

	if (OwnerCharacter->HasAuthority() == false)
		return;

	if (IsValid(Item) == false)
		return;

	const FItemData* ItemData = Item->GetData();
	checkf(ItemData != nullptr, TEXT("EquipComponent => There's no equipped item's data"));

	Item->SetOwnerCharacter(nullptr);

	UKismetSystemLibrary::PrintString(GetWorld(),
		IsValid(Item->GetOwnerCharacter()) == true ?
		*Item->GetOwnerCharacter()->GetName() : TEXT("Item No Owner"));

	TSet<UItemInstance*> ItemsToDrop;
	for (auto& EquippedItemInfo : EquippedItemInfos)
	{
		if (IsValid(EquippedItemInfo.Item) == true)
		{
			const FItemData* CompareData = EquippedItemInfo.Item->GetData();
			if (CompareData != nullptr && CompareData->ItemId != ItemData->ItemId)
				continue;
		}

		// Equipment에 이상한 값 들어있거나 유효하지 않은 상태면 정리
		// 장착한 아이템이 지울 아이템이면 정리
		if (ItemsToDrop.Find(EquippedItemInfo.Item) == nullptr)
		{
			ItemsToDrop.Add(EquippedItemInfo.Item);
		}
		EquippedItemInfo.Item = nullptr;
	}

	UClothingComponent* ClothComponent = Item->GetItemComponent<UClothingComponent>();
	if (IsValid(ClothComponent) == true)
	{
		ItemDamageReduces.Remove(Item);
		CalculateFinalDamageTakenMultiplier();
	}

	TotalWeight -= ItemData->Weight * WeightMultiplier;

	// test
	for (const auto& ItemToDrop : ItemsToDrop)
	{
		FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 40.0f;
		AItemWrapperActor* ItemWrapperActor = GetWorld()->SpawnActor<AItemWrapperActor>(
			AItemWrapperActor::StaticClass(),
			SpawnLocation,
			FRotator::ZeroRotator);

		if (IsValid(ItemWrapperActor) == true)
		{
			ItemWrapperActor->SetItemInstance(ItemToDrop);
		}
	}
	// test

	UHoldableComponent* Holdable = Item->GetItemComponent<UHoldableComponent>();
	if (IsValid(Holdable) == true)
	{
		NetMulticast_SetOwnerHoldingItemEmpty();
	}
}

void UEquipComponent::CalculateFinalDamageTakenMultiplier()
{
	FinalDamageTakenMultiplier.Empty();

	for (const auto& Pair : ItemDamageReduces)
	{
		for (const auto& DamageReduce : Pair.Value.DamageReduces)
		{
			float& RefTargetDamageTaken = FinalDamageTakenMultiplier.FindOrAdd(DamageReduce.PhysMat, 1.0f);
			RefTargetDamageTaken *= (1.0f - DamageReduce.ReduceAmount);
		}
	}
}

void UEquipComponent::OnRep_OnChangedEquippedItems()
{
	if (OnChangedEquippedItems.IsBound() == true)
	{
		OnChangedEquippedItems.Broadcast();
	}
}

void UEquipComponent::NetMulticast_SetOwnerHoldingItemEmpty_Implementation()
{
	AMyCharacter* OwnerCharacter = Cast<AMyCharacter>(GetOwner());
	if (IsValid(OwnerCharacter) == false)
		return;

	OwnerCharacter->SetHoldingItemMesh(nullptr);
	OwnerCharacter->SetAnimPreset(EHoldableType::None);
}

void UEquipComponent::ExecuteAttack(const FVector& TargetLocation)
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner) == false || Owner->HasAuthority() == false)
		return;

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

	WeaponComponent->ExecuteAttack(TargetLocation);
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

float UEquipComponent::GetFinalDamageTakenMultiplier(UPhysicalMaterial* PhysMat) const
{
	if (IsValid(PhysMat) == false)
		return 1.0f;

	const float* PtrMultiplier = FinalDamageTakenMultiplier.Find(PhysMat);
	return PtrMultiplier != nullptr ? *PtrMultiplier : 1.0f;
}
