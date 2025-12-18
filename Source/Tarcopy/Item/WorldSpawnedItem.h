// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldSpawnedItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UItemInstance;

UCLASS()
class TARCOPY_API AWorldSpawnedItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldSpawnedItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetItemInstance(UItemInstance* InItemInstance);
	UItemInstance* GetItemInstance() const { return ItemInstance; }

private:
	void RefreshFromItemInstance();

	UPROPERTY(VisibleAnywhere, Category = "World Item")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "World Item")
	TObjectPtr<USphereComponent> LootSphere;

	UPROPERTY(VisibleAnywhere, Category = "World Item")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance;

};
