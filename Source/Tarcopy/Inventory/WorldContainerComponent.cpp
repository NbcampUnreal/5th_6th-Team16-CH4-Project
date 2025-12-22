// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/WorldContainerComponent.h"

#include "Inventory/InventoryData.h"
#include "Misc/Guid.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"

UWorldContainerComponent::UWorldContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWorldContainerComponent::BeginPlay()
{
	Super::BeginPlay();

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
    }

	if (!ContainerId.IsValid())
	{
		ContainerId = FGuid::NewGuid(); // 테스트용
	}

	InventoryData = NewObject<UInventoryData>(this);
	InventoryData->Init(GridSize);
}
