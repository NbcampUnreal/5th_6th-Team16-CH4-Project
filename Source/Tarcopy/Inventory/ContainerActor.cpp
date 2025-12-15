// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ContainerActor.h"

#include "Components/BoxComponent.h"
#include "Misc/Guid.h"
#include "Inventory/InventoryData.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AContainerActor::AContainerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SenseBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SenseBox"));
	SenseBox->SetupAttachment(Root);

	SenseBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SenseBox->SetCollisionObjectType(ECC_WorldDynamic);
	SenseBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SenseBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SenseBox->SetGenerateOverlapEvents(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(Root);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetGenerateOverlapEvents(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshObject(TEXT("Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMeshObject.Succeeded())
	{
		MeshComp->SetStaticMesh(CylinderMeshObject.Object);
	}
}

// Called when the game starts or when spawned
void AContainerActor::BeginPlay()
{
	Super::BeginPlay();
	
    if (!ContainerId.IsValid())
    {
        ContainerId = FGuid::NewGuid(); // 테스트용
    }

    InventoryData = NewObject<UInventoryData>(this);
    InventoryData->Init(GridSize);
}
