// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;

UCLASS()
class TARCOPY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Character Override

public:
	AMyCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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

protected:
	UFUNCTION()
	virtual void Wheel(const FInputActionValue& Value);

	UFUNCTION()
	virtual void CanceledRightClick(const FInputActionValue& Value);
	UFUNCTION()
	virtual void TriggeredRightClick(const FInputActionValue& Value);
	UFUNCTION()
	virtual void CompletedRightClick(const FInputActionValue& Value);

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	bool bIsAttackMode;

#pragma endregion

};
