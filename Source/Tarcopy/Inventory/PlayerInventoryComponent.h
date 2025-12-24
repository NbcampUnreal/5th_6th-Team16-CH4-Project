// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class UInventoryData;
class AItemWrapperActor;
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

	void HandleRelocatePostProcess(UInventoryData* SourceInventory, UItemInstance* Item);

	void RequestDropItemToWorld(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated);

private:
	void DropItemToWorld_Internal(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated);

	UFUNCTION(Server, Reliable)
	void Server_DropItemToWorld(UInventoryData* SourceInventory, UItemInstance* Item, bool bRotated);

	UFUNCTION(Server, Reliable)
	void Server_ConsumeGroundWorldItem(UItemInstance* Item);

	ULootScannerComponent* FindLootScanner() const;

public:
	FOnPlayerInventoryReady OnInventoryReady;

private:
	UPROPERTY()
	TObjectPtr<UInventoryData> PlayerInventoryData;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FIntPoint DefaultInventorySize = FIntPoint(5, 2);

	UPROPERTY(EditDefaultsOnly, Category = "World Drop")
	TSubclassOf<AItemWrapperActor> WorldItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "World Drop")
	float DropForwardOffset = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "World Drop")
	float DropUpOffset = 10.f;
};
