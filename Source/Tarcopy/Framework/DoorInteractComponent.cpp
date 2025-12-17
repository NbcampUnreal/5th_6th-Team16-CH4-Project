// Minimal component to toggle yaw on tagged StaticMeshActors without dedicated BP actors.

#include "Framework/DoorInteractComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Character/MyCharacter.h"

UDoorInteractComponent::UDoorInteractComponent()
	: OpenYawOffset(90.f)
	, ClosedYawOffset(0.f)
	, bStartOpen(false)
	, bIsOpen(false)
	, InitialYaw(0.f)
	, bInitialized(false)
	, VisualizerOverlapCount(0)
	, bInteractionVisualizerInitialized(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDoorInteractComponent::OnRegister()
{
	Super::OnRegister();

	if (!InteractionVisualizer)
	{
		if (AActor* Owner = GetOwner())
		{
			InteractionVisualizer = NewObject<UBoxComponent>(Owner, TEXT("DoorInteractVisualizer"));
			if (InteractionVisualizer)
			{
				// 절대 트랜스폼을 사용해 문의 회전에 영향을 받지 않고 초기 위치/방향을 유지.
				InteractionVisualizer->SetUsingAbsoluteLocation(true);
				InteractionVisualizer->SetUsingAbsoluteRotation(true);
				InteractionVisualizer->SetUsingAbsoluteScale(true);

				bInteractionVisualizerInitialized = false;
				UpdateInteractionBoxFromOwner();
				InteractionVisualizer->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				InteractionVisualizer->SetGenerateOverlapEvents(true);
				InteractionVisualizer->SetCollisionResponseToAllChannels(ECR_Ignore);
				InteractionVisualizer->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
				InteractionVisualizer->SetCanEverAffectNavigation(false);
				InteractionVisualizer->SetVisibility(true, true);
				InteractionVisualizer->SetHiddenInGame(false);
				InteractionVisualizer->SetCastShadow(false);
				InteractionVisualizer->bIsEditorOnly = false;
				InteractionVisualizer->bVisualizeComponent = true;
				InteractionVisualizer->ShapeColor = FColor::Red;
				InteractionVisualizer->SetupAttachment(Owner->GetRootComponent());
				InteractionVisualizer->OnComponentBeginOverlap.AddDynamic(this, &UDoorInteractComponent::OnVisualizerBeginOverlap);
				InteractionVisualizer->OnComponentEndOverlap.AddDynamic(this, &UDoorInteractComponent::OnVisualizerEndOverlap);
				InteractionVisualizer->RegisterComponent();
			}
		}
	}
	else
	{
		UpdateInteractionBoxFromOwner();
		InteractionVisualizer->SetVisibility(true, true);
		InteractionVisualizer->SetHiddenInGame(false);
	}
}

void UDoorInteractComponent::OnUnregister()
{
	if (InteractionVisualizer)
	{
		InteractionVisualizer->DestroyComponent();
		InteractionVisualizer = nullptr;
		bInteractionVisualizerInitialized = false;
	}

	Super::OnUnregister();
}

void UDoorInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		EnsureMovableMesh();

		UpdateInteractionBoxFromOwner();
		UpdateVisualizerColor(false);

		InitialYaw = Owner->GetActorRotation().Yaw;
		bIsOpen = bStartOpen;
		bInitialized = true;
		ApplyDoorState();
	}
}

void UDoorInteractComponent::ToggleDoor()
{
	if (!bInitialized)
	{
		if (AActor* Owner = GetOwner())
		{
			EnsureMovableMesh();
			InitialYaw = Owner->GetActorRotation().Yaw;
			bIsOpen = bStartOpen;
			bInitialized = true;
		}
	}

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

void UDoorInteractComponent::EnsureMovableMesh() const
{
	if (const AActor* Owner = GetOwner())
	{
		if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Owner->GetRootComponent()))
		{
			if (Mesh->Mobility != EComponentMobility::Movable)
			{
				Mesh->SetMobility(EComponentMobility::Movable);
			}
		}
	}
}

void UDoorInteractComponent::UpdateInteractionBoxFromOwner()
{
	if (AActor* Owner = GetOwner())
	{
		if (bInteractionVisualizerInitialized)
		{
			return;
		}

		const FTransform OwnerTransform = Owner->GetActorTransform();
		const FBox LocalBounds = Owner->GetComponentsBoundingBox(true).TransformBy(OwnerTransform.Inverse());
		const FVector LocalExtent = LocalBounds.GetExtent(); // owner-local half size
		const FVector LocalCenter = LocalBounds.GetCenter(); // owner-local center
		const FRotator WorldRotation = Owner->GetActorRotation(); // capture initial facing

		if (InteractionVisualizer)
		{
			// 폭(X/Y)은 1.1배로 약간 크게, 높이(Z)는 동일하게 유지.
			const FVector AdjustedExtent(LocalExtent.X * 1.1f, LocalExtent.Y * 1.1f, LocalExtent.Z);
			InteractionVisualizer->SetBoxExtent(AdjustedExtent);
			InteractionVisualizer->SetWorldLocation(OwnerTransform.TransformPosition(LocalCenter));
			InteractionVisualizer->SetWorldRotation(WorldRotation);
			bInteractionVisualizerInitialized = true;
		}
	}
}

void UDoorInteractComponent::UpdateVisualizerColor(bool bHasCharacterInside)
{
	if (InteractionVisualizer)
	{
		InteractionVisualizer->ShapeColor = bHasCharacterInside ? FColor::Green : FColor::Red;
		InteractionVisualizer->MarkRenderStateDirty();
	}
}

void UDoorInteractComponent::OnVisualizerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->IsA<ACharacter>())
	{
		VisualizerOverlapCount++;
		UpdateVisualizerColor(true);

		if (AMyCharacter* MyChar = Cast<AMyCharacter>(OtherActor))
		{
			MyChar->AddInteractableDoor(GetOwner());
		}
	}
}

void UDoorInteractComponent::OnVisualizerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && OtherActor->IsA<ACharacter>())
	{
		VisualizerOverlapCount = FMath::Max(0, VisualizerOverlapCount - 1);
		UpdateVisualizerColor(VisualizerOverlapCount > 0);

		if (AMyCharacter* MyChar = Cast<AMyCharacter>(OtherActor))
		{
			MyChar->RemoveInteractableDoor(GetOwner());
		}
	}
}
