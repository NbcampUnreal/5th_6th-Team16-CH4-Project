// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class UInventoryData;
class AWorldSpawnedItem;
class ULootScannerComponent;
class UItemInstance;

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

	void HandleRelocatePostProcess(UInventoryData* SourceInventory, const FGuid& ItemId);

	void RequestDropItemToWorld(UInventoryData* SourceInventory, const FGuid& ItemId, bool bRotated);

private:
	void DropItemToWorld_Internal(UInventoryData* SourceInventory, const FGuid& ItemId, bool bRotated);

	UFUNCTION(Server, Reliable)
	void Server_DropItemToWorld(UInventoryData* SourceInventory, const FGuid& ItemId, bool bRotated);

	UFUNCTION(Server, Reliable)
	void Server_ConsumeGroundWorldItem(const FGuid& ItemId);

	ULootScannerComponent* FindLootScanner() const;

public:
	FOnPlayerInventoryReady OnInventoryReady;

private:
	UPROPERTY()
	TObjectPtr<UInventoryData> PlayerInventoryData;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FIntPoint DefaultInventorySize = FIntPoint(5, 2);

	UPROPERTY(EditDefaultsOnly, Category = "World Drop")
	TSubclassOf<AWorldSpawnedItem> WorldItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "World Drop")
	float DropForwardOffset = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "World Drop")
	float DropUpOffset = 10.f;
};
