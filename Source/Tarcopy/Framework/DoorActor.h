// Simple door actor that can toggle open/close with a yaw rotation.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

UCLASS()
class TARCOPY_API ADoorActor : public AActor
{
	GENERATED_BODY()

public:
	ADoorActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenYaw;

	UPROPERTY(EditAnywhere, Category = "Door")
	float ClosedYaw;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool bStartOpen;

	UPROPERTY(ReplicatedUsing = OnRep_DoorState)
	bool bIsOpen;

	UFUNCTION()
	void OnRep_DoorState();

	void ApplyDoorState();

	virtual void BeginPlay() override;
};
