// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/PlayerInventoryComponent.h"

#include "Inventory/InventoryData.h"
#include "Inventory/LootScannerComponent.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Item/ItemComponent/ItemComponentBase.h"

// Sets default values for this component's properties
UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		PlayerInventoryData = NewObject<UInventoryData>(GetOwner());
		PlayerInventoryData->Init(DefaultInventorySize);

		OnInventoryReady.Broadcast();
	}
}

void UPlayerInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerInventoryComponent, PlayerInventoryData);
}

bool UPlayerInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (IsValid(PlayerInventoryData))
	{
		bWrote |= Channel->ReplicateSubobject(PlayerInventoryData, *Bunch, *RepFlags);

		for (const FInventoryItemEntry& Entry : PlayerInventoryData->GetReplicatedItems().Items)
		{
			if (IsValid(Entry.Item))
			{
				bWrote |= Channel->ReplicateSubobject(Entry.Item, *Bunch, *RepFlags);

				const auto& ItemComponents = Entry.Item->GetItemComponents();
				for (const auto& ItemComponent : ItemComponents)
				{
					if (IsValid(ItemComponent) == false)
					{
						continue;
					}

					bWrote |= Channel->ReplicateSubobject(ItemComponent, *Bunch, *RepFlags);
				}
			}
		}
	}

	return bWrote;
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

void UPlayerInventoryComponent::RequestMoveItem(UInventoryData* Source, UItemInstance* Item, UInventoryData* Dest, FIntPoint NewOrigin, bool bRotated)
{
	if (!IsValid(Source) || !IsValid(Dest) || !IsValid(Item))
	{
		return;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_RequestMoveItem(Source, Item, Dest, NewOrigin, bRotated);
		return;
	}

	MoveItem_Internal(Source, Item, Dest, NewOrigin, bRotated);
}

void UPlayerInventoryComponent::RequestLootFromWorld(AItemWrapperActor* WorldActor, UInventoryData* Dest, FIntPoint NewOrigin, bool bRotated)
{
	if (!IsValid(WorldActor) || !IsValid(Dest))
	{
		return;
	}

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_RequestLootFromWorld(WorldActor, Dest, NewOrigin, bRotated);
		return;
	}

	Server_RequestLootFromWorld_Implementation(WorldActor, Dest, NewOrigin, bRotated);
}

void UPlayerInventoryComponent::DropItemToWorld_Internal(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

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

	Item->SetOwnerObject(nullptr);
	Spawned->SetItemInstance(Item);
	UGameplayStatics::FinishSpawningActor(Spawned, SpawnTM);

	if (ULootScannerComponent* Scanner = FindLootScanner())
	{
		Scanner->RebuildGroundInventory();
	}
}

void UPlayerInventoryComponent::MoveItem_Internal(UInventoryData* Source, UItemInstance* Item, UInventoryData* Dest, FIntPoint NewOrigin, bool bRotated)
{
	if (!IsValid(Source) || !IsValid(Dest) || !IsValid(Item))
	{
		return;
	}

	const bool bOk = Source->TryRelocateItem(Item, Dest, NewOrigin, bRotated);
	UE_LOG(LogTemp, Warning, TEXT("[Move][Server] TryRelocate=%d Src=%s Dest=%s Item=%s Origin=%s Rot=%d"),
		bOk, *GetNameSafe(Source), *GetNameSafe(Dest), *GetNameSafe(Item),
		*NewOrigin.ToString(), (int32)bRotated);
	if (!bOk)
	{
		return;
	}

	HandleRelocatePostProcess(Source, Item);
}

void UPlayerInventoryComponent::Server_RequestMoveItem_Implementation(UInventoryData* Source, UItemInstance* Item, UInventoryData* Dest, FIntPoint NewOrigin, bool bRotated)
{
	UE_LOG(LogTemp, Warning, TEXT("[MoveReq][Server] Src=%s Dest=%s Item=%s Origin=%s Rot=%d"),
		*GetNameSafe(Source), *GetNameSafe(Dest), *GetNameSafe(Item),
		*NewOrigin.ToString(), (int32)bRotated);
	MoveItem_Internal(Source, Item, Dest, NewOrigin, bRotated);
}

void UPlayerInventoryComponent::Server_RequestLootFromWorld_Implementation(AItemWrapperActor* WorldActor, UInventoryData* Dest, FIntPoint NewOrigin, bool bRotated)
{
	if (!IsValid(WorldActor) || !IsValid(Dest) || !IsValid(PlayerInventoryData))
	{
		return;
	}

	UItemInstance* Item = WorldActor->GetItemInstance();

	UE_LOG(LogTemp, Warning, TEXT("[Loot][Server] WorldActor=%s Item=%s Outer=%s"),
		*GetNameSafe(WorldActor), *GetNameSafe(Item), *GetNameSafe(Item ? Item->GetOuter() : nullptr));

	if (!IsValid(Item) || Item->GetData() == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[Loot][Server] AfterRename Outer=%s"),
		*GetNameSafe(Item->GetOuter()));

	const bool bOk = PlayerInventoryData->TryAddItem(Item, NewOrigin, bRotated);

	UE_LOG(LogTemp, Warning, TEXT("[Loot][Server] TryAdd=%d InvCount=%d"),
		bOk, PlayerInventoryData->GetReplicatedItems().Items.Num());

	UE_LOG(LogTemp, Warning, TEXT("[Server] InvData=%s HasAuthority=%d Items=%d"),
		*GetNameSafe(PlayerInventoryData),
		PlayerInventoryData->HasAnyFlags(RF_ClassDefaultObject),
		PlayerInventoryData->GetReplicatedItems().Items.Num());

	UE_LOG(LogTemp, Warning, TEXT("[Loot][Server] AddedTo=%s Num=%d"),
		*GetNameSafe(PlayerInventoryData),
		PlayerInventoryData->GetReplicatedItems().Items.Num());

	if (!bOk)
	{
		return;
	}

	WorldActor->Destroy();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->ForceNetUpdate();
	}
	Client_ForceRefreshInventoryUI();
}

ULootScannerComponent* UPlayerInventoryComponent::FindLootScanner() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<ULootScannerComponent>();
	}
	return nullptr;
}

void UPlayerInventoryComponent::OnRep_PlayerInventoryData()
{
	UE_LOG(LogTemp, Warning, TEXT("[Client] InvData=%s Items=%d"),
		*GetNameSafe(PlayerInventoryData),
		PlayerInventoryData->GetReplicatedItems().Items.Num());

	if (PlayerInventoryData)
	{
		PlayerInventoryData->FixupAfterReplication();
	}

	OnInventoryReady.Broadcast();
}

void UPlayerInventoryComponent::Client_ForceRefreshInventoryUI_Implementation()
{
	if (PlayerInventoryData)
	{
		PlayerInventoryData->FixupAfterReplication();
		PlayerInventoryData->OnInventoryChanged.Broadcast();
	}

	OnInventoryReady.Broadcast();
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
