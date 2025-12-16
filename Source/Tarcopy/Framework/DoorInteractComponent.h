// Runtime door interaction component for placed StaticMeshActors tagged as Door.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorInteractComponent.generated.h"

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
	float OpenYawOffset;

	UPROPERTY(EditAnywhere, Category = "Door")
	float ClosedYawOffset;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool bStartOpen;

	bool bIsOpen;
	float InitialYaw;
	bool bInitialized;

	UPROPERTY(Transient)
	TObjectPtr<class UBoxComponent> InteractionVisualizer;
	int32 VisualizerOverlapCount;

	void ApplyDoorState();
	void EnsureMovableMesh() const;
	void UpdateInteractionBoxFromOwner();
	void UpdateVisualizerColor(bool bHasCharacterInside);

	UFUNCTION()
	void OnVisualizerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnVisualizerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
