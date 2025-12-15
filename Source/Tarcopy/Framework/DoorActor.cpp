// Minimal door toggle actor.

#include "Framework/DoorActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ADoorActor::ADoorActor()
	: OpenYaw(90.f)
	, ClosedYaw(0.f)
	, bStartOpen(false)
	, bIsOpen(false)
{
	bReplicates = true;
	SetReplicateMovement(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
	DoorMesh->SetMobility(EComponentMobility::Movable);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	bIsOpen = bStartOpen;
	ApplyDoorState();
}

void ADoorActor::ApplyDoorState()
{
	const float TargetYaw = bIsOpen ? OpenYaw : ClosedYaw;
	FRotator NewRot = DoorMesh->GetRelativeRotation();
	NewRot.Yaw = TargetYaw;
	DoorMesh->SetRelativeRotation(NewRot);
}

void ADoorActor::ToggleDoor()
{
	bIsOpen = !bIsOpen;
	ApplyDoorState();
}

void ADoorActor::OnRep_DoorState()
{
	ApplyDoorState();
}

void ADoorActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsOpen);
}
