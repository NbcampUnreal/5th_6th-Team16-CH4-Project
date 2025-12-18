// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class UInventoryData;

DECLARE_MULTICAST_DELEGATE(FOnPlayerInventoryReady);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UInventoryData* GetPlayerInventoryData() const { return PlayerInventoryData; }

	FOnPlayerInventoryReady OnInventoryReady;

private:
	UPROPERTY()
	TObjectPtr<UInventoryData> PlayerInventoryData;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FIntPoint DefaultInventorySize = FIntPoint(5, 2);
};
