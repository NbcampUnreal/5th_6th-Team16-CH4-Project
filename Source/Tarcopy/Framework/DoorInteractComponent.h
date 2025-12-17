// Runtime door interaction component for placed StaticMeshActors tagged as Door.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorInteractComponent.generated.h"

UENUM(BlueprintType)
enum class EDoorMotionType : uint8
{
	SwingYaw UMETA(DisplayName = "Swing (Yaw)"),
	Slide UMETA(DisplayName = "Slide (Translate)"),
};

UCLASS(ClassGroup = (Interaction), Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class TARCOPY_API UDoorInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDoorInteractComponent();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Door")
	EDoorMotionType MotionType = EDoorMotionType::SwingYaw;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenYawOffset;

	UPROPERTY(EditAnywhere, Category = "Door")
	float ClosedYawOffset;

	UPROPERTY(EditAnywhere, Category = "Door", meta = (EditCondition = "MotionType == EDoorMotionType::Slide"))
	FVector OpenLocalOffset = FVector(100.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Door", meta = (EditCondition = "MotionType == EDoorMotionType::Slide"))
	FVector ClosedLocalOffset = FVector(0.f, 0.f, 0.f);

	// When enabled, the component computes a slide direction and distance automatically from the owner's bounds.
	// This makes the door slide approximately by its own size.
	UPROPERTY(EditAnywhere, Category = "Door", meta = (EditCondition = "MotionType == EDoorMotionType::Slide"))
	bool bAutoSlideFromBounds = true;

	// Multiplier applied to the computed bound-based slide distance (1.0 = exactly the door size).
	UPROPERTY(EditAnywhere, Category = "Door", meta = (EditCondition = "MotionType == EDoorMotionType::Slide"))
	float AutoSlideDistanceMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool bStartOpen;

	// If set, interacting with this door will also apply the same open/close state
	// to other doors with the same group tag.
	// If left empty, the component will infer the group tag from the owner's Actor Tags
	// using the prefix "DoorGroup_" (e.g. DoorGroup_LobbyA).
	UPROPERTY(EditAnywhere, Category = "Door")
	FName DoorGroupTag;

	bool bIsOpen;
	FVector InitialLocation;
	FRotator InitialRotation;
	float InitialYaw;
	bool bInitialized;

	UPROPERTY(Transient)
	TObjectPtr<class UBoxComponent> InteractionVisualizer;
	int32 VisualizerOverlapCount;
	bool bInteractionVisualizerInitialized = false;

	void ApplyDoorState();
	void EnsureMovableMesh() const;
	void InitializeIfNeeded();
	void SetDoorOpenInternal(bool bOpen);
	void ToggleDoorInternal(bool bPropagateToGroup);
	FName ResolveDoorGroupTag() const;
	void RefreshAutoSlideOffsetsFromBounds();
	void UpdateInteractionBoxFromOwner();
	void UpdateVisualizerColor(bool bHasCharacterInside);

	UFUNCTION()
	void OnVisualizerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnVisualizerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
