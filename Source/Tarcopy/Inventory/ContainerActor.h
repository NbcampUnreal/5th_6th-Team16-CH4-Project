// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ContainerActor.generated.h"

class UBoxComponent;
class UInventoryData;

UCLASS()
class TARCOPY_API AContainerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AContainerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FGuid GetContainerId() const { return ContainerId; }
	FText GetDisplayName() const { return DisplayName; }
	UInventoryData* GetInventoryData() const { return InventoryData; }

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> SenseBox;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> MeshComp;

    UPROPERTY(EditAnywhere, Category = "Container")
    FText DisplayName = FText::FromString(TEXT("Container"));

    UPROPERTY(EditAnywhere, Category = "Container")
    FIntPoint GridSize = FIntPoint(5, 6);

    UPROPERTY(EditAnywhere, Category = "Container")
    FGuid ContainerId;

    UPROPERTY()
    TObjectPtr<UInventoryData> InventoryData;
};
