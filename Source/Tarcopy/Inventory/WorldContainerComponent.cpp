// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/WorldContainerComponent.h"

#include "Inventory/InventoryData.h"
#include "Misc/Guid.h"

UWorldContainerComponent::UWorldContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWorldContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ContainerId.IsValid())
	{
		ContainerId = FGuid::NewGuid(); // 테스트용
	}

	InventoryData = NewObject<UInventoryData>(this);
	InventoryData->Init(GridSize);
}
