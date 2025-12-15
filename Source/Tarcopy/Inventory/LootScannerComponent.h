// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LootScannerComponent.generated.h"

class USphereComponent;
class AContainerActor;
class AWorldSpawnedItem;
class UInventoryData;

DECLARE_MULTICAST_DELEGATE(FOnScannedContainersChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API ULootScannerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULootScannerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnScannedContainersChanged OnScannedContainersChanged;

	UPROPERTY()
	TSet<TWeakObjectPtr<AContainerActor>> OverlappedContainerActors;

private:
	UFUNCTION()
	void OnContainerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnContainerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnGroundBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGroundEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "Loot Scan")
	TObjectPtr<USphereComponent> ContainerSense;

	UPROPERTY(VisibleAnywhere, Category = "Loot Scan")
	TObjectPtr<USphereComponent> GroundSense;

	UPROPERTY(EditAnywhere, Category = "Loot Scan")
	float ContainerScanRadius = 800.f;

	UPROPERTY(EditAnywhere, Category = "Loot Scan")
	float GroundScanRadius = 600.f;

	UPROPERTY(EditAnywhere, Category = "Loot Scan")
	FIntPoint GroundGridSize = FIntPoint(10, 10);

	UPROPERTY()
	TSet<TWeakObjectPtr<AWorldSpawnedItem>> OverlappedGroundItems;

	UPROPERTY()
	TObjectPtr<UInventoryData> GroundInventoryData;

	UPROPERTY()
	TMap<FGuid, TWeakObjectPtr<AWorldSpawnedItem>> InstanceIdToWorldItem;
};
