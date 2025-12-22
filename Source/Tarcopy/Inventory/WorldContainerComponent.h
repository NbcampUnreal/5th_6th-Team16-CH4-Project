// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldContainerComponent.generated.h"

class UInventoryData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TARCOPY_API UWorldContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWorldContainerComponent();

	FGuid GetContainerId() const { return ContainerId; }
	FText GetDisplayName() const { return DisplayName; }
	FIntPoint GetGridSize() const { return GridSize; }
	UInventoryData* GetInventoryData() const { return InventoryData; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Container")
	FText DisplayName = FText::FromString(TEXT("Container"));

	UPROPERTY(EditAnywhere, Category = "Container")
	FIntPoint GridSize = FIntPoint(5, 6);

	UPROPERTY(EditAnywhere, Category = "Container")
	FGuid ContainerId;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryData> InventoryData;
};
