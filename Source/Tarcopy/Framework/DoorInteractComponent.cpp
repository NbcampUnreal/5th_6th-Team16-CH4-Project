// Minimal component to toggle yaw on tagged StaticMeshActors without dedicated BP actors.

#include "Framework/DoorInteractComponent.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"

UDoorInteractComponent::UDoorInteractComponent()
	: OpenYawOffset(90.f)
	, ClosedYawOffset(0.f)
	, bStartOpen(false)
	, bIsOpen(false)
	, InitialLocation(FVector::ZeroVector)
	, InitialRotation(FRotator::ZeroRotator)
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
				InteractionVisualizer->SetVisibility(false, true);
				InteractionVisualizer->SetHiddenInGame(true);
				InteractionVisualizer->SetCastShadow(false);
				InteractionVisualizer->bIsEditorOnly = false;
				InteractionVisualizer->bVisualizeComponent = false;
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
		InteractionVisualizer->SetVisibility(false, true);
		InteractionVisualizer->SetHiddenInGame(true);
		InteractionVisualizer->bVisualizeComponent = false;
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

		static const FName DoorSlideTag(TEXT("DoorSlide"));
		if (Owner->ActorHasTag(DoorSlideTag))
		{
			MotionType = EDoorMotionType::Slide;
		}

		UpdateInteractionBoxFromOwner();
		// 상호작용 영역(Visualizer) 표시는 비활성화합니다.
		// UpdateVisualizerColor(false);

		InitialLocation = Owner->GetActorLocation();
		InitialRotation = Owner->GetActorRotation();
		InitialYaw = InitialRotation.Yaw;
		RefreshAutoSlideOffsetsFromBounds();
		bIsOpen = bStartOpen;
		bInitialized = true;
		ApplyDoorState();
	}
}

void UDoorInteractComponent::ToggleDoor()
{
	ToggleDoorInternal(true);
}

void UDoorInteractComponent::ApplyDoorState()
{
	if (AActor* Owner = GetOwner())
	{
		if (MotionType == EDoorMotionType::Slide)
		{
			const FVector LocalOffset = bIsOpen ? OpenLocalOffset : ClosedLocalOffset;
			const FVector WorldOffset = InitialRotation.RotateVector(LocalOffset);
			Owner->SetActorLocation(InitialLocation + WorldOffset);
		}
		else
		{
			FRotator Rot = InitialRotation;
			Rot.Yaw = InitialYaw + (bIsOpen ? OpenYawOffset : ClosedYawOffset);
			Owner->SetActorRotation(Rot);
		}
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

FName UDoorInteractComponent::ResolveDoorGroupTag() const
{
	if (DoorGroupTag != NAME_None)
	{
		return DoorGroupTag;
	}

	static const FString DoorGroupPrefix(TEXT("DoorGroup_"));
	if (const AActor* Owner = GetOwner())
	{
		for (const FName& Tag : Owner->Tags)
		{
			if (Tag.ToString().StartsWith(DoorGroupPrefix))
			{
				return Tag;
			}
		}
	}

	return NAME_None;
}

void UDoorInteractComponent::InitializeIfNeeded()
{
	if (bInitialized)
	{
		return;
	}

	if (AActor* Owner = GetOwner())
	{
		EnsureMovableMesh();

		static const FName DoorSlideTag(TEXT("DoorSlide"));
		if (Owner->ActorHasTag(DoorSlideTag))
		{
			MotionType = EDoorMotionType::Slide;
		}

		InitialLocation = Owner->GetActorLocation();
		InitialRotation = Owner->GetActorRotation();
		InitialYaw = InitialRotation.Yaw;
		RefreshAutoSlideOffsetsFromBounds();
		bIsOpen = bStartOpen;
		bInitialized = true;
	}
}

void UDoorInteractComponent::RefreshAutoSlideOffsetsFromBounds()
{
	if (MotionType != EDoorMotionType::Slide || !bAutoSlideFromBounds)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	const FTransform OwnerTransform = Owner->GetActorTransform();
	const FBox LocalBounds = Owner->GetComponentsBoundingBox(true).TransformBy(OwnerTransform.Inverse());
	const FVector LocalExtent = LocalBounds.GetExtent(); // owner-local half size

	// FVector(UE::Math::TVector<double>) does not provide GetMinAxis/GetMaxAxis in UE5,
	// so we compute the axis manually.
	const int32 ThicknessAxis =
		(LocalExtent.X <= LocalExtent.Y && LocalExtent.X <= LocalExtent.Z) ? 0 :
		((LocalExtent.Y <= LocalExtent.Z) ? 1 : 2);

	// Choose slide axis from the two non-thickness axes, preferring a horizontal axis (X/Y) over vertical (Z).
	TArray<int32> CandidateAxes;
	CandidateAxes.Reserve(2);
	for (int32 Axis = 0; Axis < 3; ++Axis)
	{
		if (Axis != ThicknessAxis)
		{
			CandidateAxes.Add(Axis);
		}
	}

	int32 SlideAxis = CandidateAxes[0];
	if (CandidateAxes.Contains(2) && SlideAxis == 2)
	{
		SlideAxis = CandidateAxes[1];
	}
	else if (!CandidateAxes.Contains(2) || CandidateAxes[1] != 2)
	{
		SlideAxis = (LocalExtent[CandidateAxes[1]] > LocalExtent[CandidateAxes[0]]) ? CandidateAxes[1] : CandidateAxes[0];
	}

	FVector LocalDir = FVector::ZeroVector;
	LocalDir[SlideAxis] = 1.0f;

	static const FName DoorSlideReverseTag(TEXT("DoorSlideReverse"));
	if (Owner->ActorHasTag(DoorSlideReverseTag))
	{
		LocalDir *= -1.0f;
	}

	const float SlideDistance = LocalExtent[SlideAxis] * 2.0f * AutoSlideDistanceMultiplier;

	ClosedLocalOffset = FVector::ZeroVector;
	OpenLocalOffset = LocalDir * SlideDistance;
}

void UDoorInteractComponent::SetDoorOpenInternal(bool bOpen)
{
	InitializeIfNeeded();
	bIsOpen = bOpen;
	ApplyDoorState();
}

void UDoorInteractComponent::ToggleDoorInternal(bool bPropagateToGroup)
{
	InitializeIfNeeded();

	const bool bNewOpen = !bIsOpen;

	if (!bPropagateToGroup)
	{
		SetDoorOpenInternal(bNewOpen);
		return;
	}

	static const FName DoorTag(TEXT("Door"));
	const FName GroupTag = ResolveDoorGroupTag();
	if (GroupTag == NAME_None)
	{
		SetDoorOpenInternal(bNewOpen);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		SetDoorOpenInternal(bNewOpen);
		return;
	}

	TArray<AActor*> GroupActors;
	UGameplayStatics::GetAllActorsWithTag(World, GroupTag, GroupActors);
	if (GroupActors.IsEmpty())
	{
		SetDoorOpenInternal(bNewOpen);
		return;
	}

	for (AActor* DoorActor : GroupActors)
	{
		if (!IsValid(DoorActor) || !DoorActor->ActorHasTag(DoorTag))
		{
			continue;
		}

		UDoorInteractComponent* DoorComp = DoorActor->FindComponentByClass<UDoorInteractComponent>();
		if (!DoorComp)
		{
			DoorComp = NewObject<UDoorInteractComponent>(DoorActor);
			if (!IsValid(DoorComp))
			{
				continue;
			}
			DoorComp->RegisterComponent();
		}

		DoorComp->SetDoorOpenInternal(bNewOpen);
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
