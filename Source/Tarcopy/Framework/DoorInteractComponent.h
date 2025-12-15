// Door interaction component to let existing StaticMeshActors toggle open/close when tagged.

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

	void ApplyDoorState();
};
