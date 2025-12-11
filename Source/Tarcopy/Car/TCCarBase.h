// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "TCCarBase.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class USpotLightComponent;
class UTCCarCombatComponent;
struct FInputActionValue;

UCLASS(abstract)
class ATCCarBase : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Light;

	UPROPERTY(EditDefaultsOnly)
	USpotLightComponent* HeadLight_R;

	UPROPERTY(EditDefaultsOnly)
	USpotLightComponent* HeadLight_L;
 
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTCCarCombatComponent> CombatComponent;


protected:

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* BrakeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* HandbrakeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LightAction;

public:
	ATCCarBase();

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float Delta) override;


protected:

	void Steering(const FInputActionValue& Value);

	void Throttle(const FInputActionValue& Value);

	void Brake(const FInputActionValue& Value);

	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	void ToggleLight(const FInputActionValue& Value);


public:

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoSteering(float SteeringValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoThrottle(float ThrottleValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoBrake(float BrakeValue);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoBrakeStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoBrakeStop();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoHandbrakeStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoHandbrakeStop();

	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoHandLight();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void BrakeLights(bool bBraking);

	UFUNCTION()
	void DamageOn();

public:
	bool bLightOn;

public:
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return Camera; }
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};
