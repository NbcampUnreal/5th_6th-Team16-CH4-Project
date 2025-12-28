// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class USphereComponent;
class UDoorInteractComponent;
struct FInputActionValue;
class UMoodleComponent;
class UCameraObstructionComponent;
enum class EHoldableType : uint8;
class UAnimPresetMap;

UCLASS()
class TARCOPY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Character Override

public:
	AMyCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_Controller() override;

protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Viewport Components

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Equip", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEquipComponent> EquipComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Equip", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHealthComponent> HealthComponent;

	UFUNCTION()
	virtual void OnInteractionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnInteractionSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnVisionMeshBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnVisionMeshEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraObstructionComponent> CameraObstruction;

public:
	void AddInteractableDoor(AActor* DoorActor);
	void RemoveInteractableDoor(AActor* DoorActor);

protected:
	TSet<TWeakObjectPtr<AActor>> OverlappingDoors;

#pragma endregion

#pragma region State Components

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMoodleComponent> Moodle;

public:
	UMoodleComponent* GetMoodleComponent() const { return Moodle; }

#pragma endregion

#pragma region MoveAction
protected:
	UFUNCTION()
	virtual void MoveAction(const FInputActionValue& Value);

	UFUNCTION()
	virtual void StartSprint(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_SetSpeed(float InSpeed);
	UFUNCTION()
	virtual void StopSprint(const FInputActionValue& Value);
	UFUNCTION()
	virtual void OnRep_SetSpeed();

	UFUNCTION()
	virtual void StartCrouch(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_Crouch();
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPC_Crouch();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float SprintSpeedMultiplier;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Speed", meta = (AllowPrivateAccess = "true"))
	float CrouchSpeedMultiplier;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_SetSpeed)
	float CurrentSpeed;

#pragma endregion

#pragma region Mouse Action

public:
	bool IsAiming() { return bIsAttackMode; }

protected:
	UFUNCTION()
	virtual void Wheel(const FInputActionValue& Value);

	UFUNCTION()
	virtual void CanceledRightClick(const FInputActionValue& Value);
	UFUNCTION()
	virtual void TriggeredRightClick(const FInputActionValue& Value);
	UFUNCTION()
	virtual void CompletedRightClick(const FInputActionValue& Value);

	UFUNCTION()
	virtual void LeftClick(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_ExecuteAttack(const FVector& TargetLocation);
	FVector GetAttackTargetLocation() const;

	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_TurnToMouse(const FRotator& TargetRot);
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPC_TurnToMouse(const FRotator& TargetRot);
	virtual void TurnToMouse();

	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_StopTurnToMouse();
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPC_StopTurnToMouse();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsAttackMode;

	UFUNCTION()
	virtual void Interact(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	virtual void ServerRPC_ToggleDoor(AActor* DoorActor);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPC_ApplyDoorTransforms(const TArray<AActor*>& DoorActors, const TArray<FTransform>& DoorTransforms, const TArray<bool>& DoorOpenStates);

#pragma endregion

#pragma region Combat
	public:
		virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TObjectPtr<UAnimMontage> AM_Hit;

		UPROPERTY(Replicated, ReplicatedUsing = "OnRep_bIsHit")
		bool bIsHit;

		UFUNCTION()
		void OnRep_bIsHit();
		UFUNCTION()
		void OnHitMontageEnded(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion

#pragma region TestItem

public:
	UFUNCTION()
	void SetItem();

	UFUNCTION()
	bool GetAimTarget(AActor*& OutTargetActor, FName& OutBone) const;

	void SetHoldingItemMesh(UStaticMesh* ItemMeshAsset, const FName& SocketName = NAME_None);
	void SetAnimPreset(EHoldableType Type);

	void GetNearbyInventoryDatas(TArray<class UInventoryData*>& InventoryDatas);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> HoldingItemMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimPresetMap> AnimPresetMap;

#pragma endregion


#pragma region Inventory
	UFUNCTION()
	virtual void TabAction(const FInputActionValue& Value);

	void OnRotateInventoryItem();
#pragma endregion
};
