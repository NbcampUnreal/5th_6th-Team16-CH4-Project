// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ContainerActor.generated.h"

class UBoxComponent;
class UWorldContainerComponent;
class UInventoryData;

UCLASS()
class TARCOPY_API AContainerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AContainerActor();

public:
	UWorldContainerComponent* GetContainerComponent() const { return ContainerComponent; }
	FGuid GetContainerId() const;
	FText GetDisplayName() const;
	UInventoryData* GetInventoryData() const;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWorldContainerComponent> ContainerComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> SenseBox;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> MeshComp;
};
