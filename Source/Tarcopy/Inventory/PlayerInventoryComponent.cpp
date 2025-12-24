// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PlayerInventoryComponent.h"

#include "Inventory/InventoryData.h"
#include "Inventory/LootScannerComponent.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"
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

void UPlayerInventoryComponent::HandleRelocatePostProcess(UInventoryData* SourceInventory, UItemInstance* Item)
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
			Scanner->ConsumeGroundWorldItem(Item);
		}
		else
		{
			Server_ConsumeGroundWorldItem(Item);
		}
	}
}

void UPlayerInventoryComponent::RequestDropItemToWorld(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated)
{
	if (!IsValid(SourceInventory) || !IsValid(Item))
	{
		return;
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		DropItemToWorld_Internal(SourceInventory, Item, bRotated);
	}
	else
	{
		Server_DropItemToWorld(SourceInventory, Item, bRotated);
	}
}

void UPlayerInventoryComponent::DropItemToWorld_Internal(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated)
{
	//if (!GetOwner() || !GetOwner()->HasAuthority())
	//{
	//	return;
	//}

	if (!IsValid(SourceInventory) || !IsValid(Item))
    {
        return;
    }

	SourceInventory->RemoveItem(Item);

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

	AItemWrapperActor* Spawned = GetWorld()->SpawnActorDeferred<AItemWrapperActor>(
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

	Spawned->SetItemInstance(Item);
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

void UPlayerInventoryComponent::Server_ConsumeGroundWorldItem_Implementation(UItemInstance* Item)
{
	ULootScannerComponent* Scanner = FindLootScanner();
	if (!Scanner)
	{
		return;
	}

	Scanner->ConsumeGroundWorldItem(Item);
}

void UPlayerInventoryComponent::Server_DropItemToWorld_Implementation(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated)
{
	DropItemToWorld_Internal(SourceInventory, Item, bRotated);
}
