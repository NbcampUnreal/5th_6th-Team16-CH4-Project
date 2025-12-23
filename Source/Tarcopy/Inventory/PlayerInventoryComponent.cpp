// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PlayerInventoryComponent.h"

#include "Inventory/InventoryData.h"
#include "Inventory/LootScannerComponent.h"
#include "Item/WorldSpawnedItem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemInstance.h"

// Sets default values for this component's properties
UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerInventoryData = NewObject<UInventoryData>(this);
	PlayerInventoryData->Init(DefaultInventorySize);

	OnInventoryReady.Broadcast();
}

void UPlayerInventoryComponent::HandleRelocatePostProcess(UInventoryData* SourceInventory, const FGuid& ItemId)
{
	if (!SourceInventory)
	{
		return;
	}

	ULootScannerComponent* Scanner = FindLootScanner();
	if (!Scanner)
	{
		return;
	}

	if (SourceInventory == Scanner->GetGroundInventoryData())
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			Scanner->ConsumeGroundWorldItemByInstanceId(ItemId);
		}
		else
		{
			Server_ConsumeGroundWorldItem(ItemId);
		}
	}
}

void UPlayerInventoryComponent::RequestDropItemToWorld(UInventoryData* SourceInventory, const FGuid& ItemId, bool bRotated)
{
	if (!SourceInventory)
	{
		return;
	}

	DropItemToWorld_Internal(SourceInventory, ItemId, bRotated);
	//if (GetOwner() && GetOwner()->HasAuthority())
	//{
	//	DropItemToWorld_Internal(SourceInventory, ItemId, bRotated);
	//}
	//else
	//{
	//	Server_DropItemToWorld(SourceInventory, ItemId, bRotated);
	//}
}

void UPlayerInventoryComponent::DropItemToWorld_Internal(UInventoryData* SourceInventory, const FGuid& ItemId, bool bRotated)
{
	//if (!GetOwner() || !GetOwner()->HasAuthority())
	//{
	//	return;
	//}

	if (!SourceInventory)
	{
		return;
	}

	UItemInstance* ItemInst = SourceInventory->FindItemByID(ItemId);
	if (!IsValid(ItemInst))
	{
		return;
	}

	SourceInventory->RemoveItemByInstanceId(ItemId);

	if (!WorldItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropItemToWorld_Internal: WorldItemClass is null"));
		return;
	}

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	const FVector BaseLoc = OwnerChar ? OwnerChar->GetActorLocation() : GetOwner()->GetActorLocation();
	const FVector Forward = OwnerChar ? OwnerChar->GetActorForwardVector() : GetOwner()->GetActorForwardVector();

	const FVector SpawnLoc = BaseLoc + Forward * DropForwardOffset + FVector(0.f, 0.f, DropUpOffset);
	const FRotator SpawnRot = FRotator::ZeroRotator;

	FTransform SpawnTM(SpawnRot, SpawnLoc);

	AWorldSpawnedItem* Spawned = GetWorld()->SpawnActorDeferred<AWorldSpawnedItem>(
		WorldItemClass,
		SpawnTM,
		GetOwner(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (!IsValid(Spawned))
	{
		return;
	}

	Spawned->SetItemInstance(ItemInst);
	UGameplayStatics::FinishSpawningActor(Spawned, SpawnTM);

	if (ULootScannerComponent* Scanner = FindLootScanner())
	{
		Scanner->RebuildGroundInventory();
	}
}

ULootScannerComponent* UPlayerInventoryComponent::FindLootScanner() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<ULootScannerComponent>();
	}
	return nullptr;
}

void UPlayerInventoryComponent::Server_ConsumeGroundWorldItem_Implementation(const FGuid& ItemId)
{
	ULootScannerComponent* Scanner = FindLootScanner();
	if (!Scanner)
	{
		return;
	}

	Scanner->ConsumeGroundWorldItemByInstanceId(ItemId);
}

void UPlayerInventoryComponent::Server_DropItemToWorld_Implementation(UInventoryData* SourceInventory, const FGuid& ItemId, bool bRotated)
{
	DropItemToWorld_Internal(SourceInventory, ItemId, bRotated);
}
