// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/WorldContainerComponent.h"

#include "Inventory/InventoryData.h"
#include "Misc/Guid.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Item/ItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "Item/ItemWrapperActor/ItemWrapperActor.h"
#include "AI/MyAICharacter.h"
#include "Item/ItemSpawnSubsystem.h"

UWorldContainerComponent::UWorldContainerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UWorldContainerComponent::BeginPlay()
{
	Super::BeginPlay();

    //if (!GetOwner() || !GetOwner()->HasAuthority())
    //{
    //    return;
    //}

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    if (!SenseBox)
    {
        SenseBox = NewObject<UBoxComponent>(Owner, TEXT("SenseBox"));
        if (!SenseBox) return;

        SenseBox->SetupAttachment(this);
        SenseBox->SetRelativeLocation(FVector::ZeroVector);
        SenseBox->SetRelativeRotation(FRotator::ZeroRotator);
        SenseBox->SetRelativeScale3D(FVector::OneVector);

        Owner->AddInstanceComponent(SenseBox);
        SenseBox->RegisterComponent();

        SenseBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));

        SenseBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        SenseBox->SetGenerateOverlapEvents(true);
        SenseBox->SetCollisionObjectType(ECC_GameTraceChannel1);

        SenseBox->SetCollisionResponseToAllChannels(ECR_Ignore);
        SenseBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

        AMyAICharacter* AI = GetOwner<AMyAICharacter>();
        if (IsValid(AI))
        {
            SenseBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
        }
    }

	if (!ContainerId.IsValid())
	{
		ContainerId = FGuid::NewGuid(); // 테스트용
	}

	InventoryData = NewObject<UInventoryData>(this);
	InventoryData->Init(GridSize);

    /*UItemInstance* NewItem = NewObject<UItemInstance>(InventoryData);
    NewItem->SetItemId(TEXT("Axe1"));

    InventoryData->TryAddItem(NewItem, FIntPoint::ZeroValue, false);*/

    UItemSpawnSubsystem* ItemSpawnSubsystem = GetWorld()->GetSubsystem<UItemSpawnSubsystem>();
    if (IsValid(ItemSpawnSubsystem) == false)
        return;

    ItemSpawnSubsystem->SpawnItemAtGround(Owner, FName("Axe1"));

    GetOwner()->ForceNetUpdate();
}

void UWorldContainerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, InventoryData);
}

bool UWorldContainerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool bWrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    if (IsValid(InventoryData))
    {
        bWrote |= Channel->ReplicateSubobject(InventoryData, *Bunch, *RepFlags);

        for (const FInventoryItemEntry& Entry : InventoryData->GetReplicatedItems().Items)
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

void UWorldContainerComponent::OnRep_InventoryData()
{
    if (InventoryData)
    {
        InventoryData->FixupAfterReplication();
    }
}
