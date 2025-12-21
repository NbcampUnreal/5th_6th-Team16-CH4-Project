// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/LootScannerComponent.h"

#include "Components/SphereComponent.h"
#include "Inventory/InventoryData.h"
#include "Item/WorldSpawnedItem.h"
#include "Inventory/ContainerActor.h"
#include "Item/ItemInstance.h"
#include "Item/ItemComponent/ContainerComponent.h"

// Sets default values for this component's properties
ULootScannerComponent::ULootScannerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ContainerSense = CreateDefaultSubobject<USphereComponent>(TEXT("ContainerSense"));
	ContainerSense->SetupAttachment(this);
	ContainerSense->InitSphereRadius(ContainerScanRadius);
	ContainerSense->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ContainerSense->SetCollisionObjectType(ECC_WorldDynamic);
	ContainerSense->SetCollisionResponseToAllChannels(ECR_Ignore);
	ContainerSense->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	ContainerSense->SetGenerateOverlapEvents(true);

	GroundSense = CreateDefaultSubobject<USphereComponent>(TEXT("GroundSense"));
	GroundSense->SetupAttachment(this);
	GroundSense->InitSphereRadius(GroundScanRadius);
	GroundSense->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GroundSense->SetCollisionObjectType(ECC_WorldDynamic);
	GroundSense->SetCollisionResponseToAllChannels(ECR_Ignore);
	GroundSense->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GroundSense->SetGenerateOverlapEvents(true);
}


// Called when the game starts
void ULootScannerComponent::BeginPlay()
{
	Super::BeginPlay();

	ContainerSense->SetSphereRadius(ContainerScanRadius);
	GroundSense->SetSphereRadius(GroundScanRadius);

	ContainerSense->OnComponentBeginOverlap.AddDynamic(this, &ULootScannerComponent::OnContainerBeginOverlap);
	ContainerSense->OnComponentEndOverlap.AddDynamic(this, &ULootScannerComponent::OnContainerEndOverlap);

	GroundSense->OnComponentBeginOverlap.AddDynamic(this, &ULootScannerComponent::OnGroundBeginOverlap);
	GroundSense->OnComponentEndOverlap.AddDynamic(this, &ULootScannerComponent::OnGroundEndOverlap);

	GroundInventoryData = NewObject<UInventoryData>(this);
	GroundInventoryData->Init(GroundGridSize);
}

void ULootScannerComponent::RebuildGroundInventory()
{
	if (!GroundInventoryData)
	{
		return;
	}

	GroundInventoryData->ClearAll();

	for (const TWeakObjectPtr<AWorldSpawnedItem>& ItemActorPtr : OverlappedGroundItems)
	{
		AWorldSpawnedItem* ItemActor = ItemActorPtr.Get();
		if (!IsValid(ItemActor))
		{
			continue;
		}

		UItemInstance* ItemInstance = ItemActor->GetItemInstance();
		if (!IsValid(ItemInstance))
		{
			continue;
		}

		bool bPlaced = false;

		for (int32 Y = 0; Y < GroundGridSize.Y && !bPlaced; ++Y)
		{
			for (int32 X = 0; X < GroundGridSize.X && !bPlaced; ++X)
			{
				if (GroundInventoryData->TryAddItem(ItemInstance, FIntPoint(X, Y), false))
				{
					bPlaced = true;
				}
			}
		}
	}

	OnScannedGroundChanged.Broadcast();
}

void ULootScannerComponent::OnContainerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (AContainerActor* Container = Cast<AContainerActor>(OtherActor))
	{
		OverlappedContainerActors.Add(Container);
		OnScannedContainersChanged.Broadcast();
	}
}

void ULootScannerComponent::OnContainerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	if (AContainerActor* Container = Cast<AContainerActor>(OtherActor))
	{
		OverlappedContainerActors.Remove(Container);
		OnScannedContainersChanged.Broadcast();
	}
}

void ULootScannerComponent::OnGroundBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (AWorldSpawnedItem* ItemActor = Cast<AWorldSpawnedItem>(OtherActor))
	{
		const UContainerComponent* ContainerComp = ItemActor->GetItemInstance()->GetItemComponent<UContainerComponent>();
		if (ContainerComp)
		{
			OverlappedContainerItems.Add(ItemActor);
			OnScannedContainersChanged.Broadcast();
		}
		OverlappedGroundItems.Add(ItemActor);
		RebuildGroundInventory();
	}
}

void ULootScannerComponent::OnGroundEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	if (AWorldSpawnedItem* ItemActor = Cast<AWorldSpawnedItem>(OtherActor))
	{
		const UContainerComponent* ContainerComp = ItemActor->GetItemInstance()->GetItemComponent<UContainerComponent>();
		if (ContainerComp)
		{
			OverlappedContainerItems.Remove(ItemActor);
			OnScannedContainersChanged.Broadcast();
		}
		OverlappedGroundItems.Remove(ItemActor);
		RebuildGroundInventory();
	}
}

