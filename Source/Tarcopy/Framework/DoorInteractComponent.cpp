// Minimal component to turn a tagged StaticMeshActor into an interactable door.

#include "Framework/DoorInteractComponent.h"
#include "GameFramework/Actor.h"

UDoorInteractComponent::UDoorInteractComponent()
	: OpenYawOffset(90.f)
	, ClosedYawOffset(0.f)
	, bStartOpen(false)
	, bIsOpen(false)
	, InitialYaw(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDoorInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		InitialYaw = Owner->GetActorRotation().Yaw;
		bIsOpen = bStartOpen;
		ApplyDoorState();
	}
}

void UDoorInteractComponent::ToggleDoor()
{
	bIsOpen = !bIsOpen;
	ApplyDoorState();
}

void UDoorInteractComponent::ApplyDoorState()
{
	if (AActor* Owner = GetOwner())
	{
		FRotator Rot = Owner->GetActorRotation();
		Rot.Yaw = InitialYaw + (bIsOpen ? OpenYawOffset : ClosedYawOffset);
		Owner->SetActorRotation(Rot);
	}
}
