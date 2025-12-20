#include "Item/ItemWrapperActor/ItemWrapperActor.h"
#include "Components/SphereComponent.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"

AItemWrapperActor::AItemWrapperActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	LootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LootSphere"));
	LootSphere->SetupAttachment(SceneRoot);
	LootSphere->InitSphereRadius(50.f);
	LootSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LootSphere->SetCollisionObjectType(ECC_WorldDynamic);
	LootSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	LootSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	LootSphere->SetGenerateOverlapEvents(true);

	DefaultMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultMesh"));
	DefaultMesh->SetupAttachment(SceneRoot);
	DefaultMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DefaultMesh->SetGenerateOverlapEvents(false);
}

void AItemWrapperActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemWrapperActor::SetItemInstance(UItemInstance* InItemInstance)
{
	if (IsValid(InItemInstance) == false)
		return;

	ItemInstance = InItemInstance;
	const FItemData* ItemData = ItemInstance->GetData();
	if (ItemData == nullptr)
		return;
	
	UStaticMesh* MeshAsset = ItemData->DefaultMesh;
	if (IsValid(MeshAsset) == false)
		return;

	DefaultMesh->SetStaticMesh(MeshAsset);
}
