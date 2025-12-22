// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WorldSpawnedItem.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWorldSpawnedItem::AWorldSpawnedItem()
{
	PrimaryActorTick.bCanEverTick = false;

	//bReplicates = true;
	//SetReplicateMovement(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	LootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LootSphere"));
	LootSphere->SetupAttachment(Root);

	LootSphere->InitSphereRadius(50.f);
	LootSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LootSphere->SetCollisionObjectType(ECC_WorldDynamic);
	LootSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	LootSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	LootSphere->SetGenerateOverlapEvents(true);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(Root);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetGenerateOverlapEvents(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMeshObject(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeMeshObject.Succeeded())
	{
		MeshComp->SetStaticMesh(ConeMeshObject.Object);
	}
}

// Called when the game starts or when spawned
void AWorldSpawnedItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldSpawnedItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldSpawnedItem::SetItemInstance(UItemInstance* InItemInstance)
{
	ItemInstance = InItemInstance;
	RefreshFromItemInstance();
}

void AWorldSpawnedItem::RefreshFromItemInstance()
{
	if (!ItemInstance)
	{
		return;
	}
}

