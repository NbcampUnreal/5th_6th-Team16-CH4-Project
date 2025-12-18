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
class UTCCarWidget;
class UUISubsystem;
struct FInputActionValue;

UCLASS(abstract)
class ATCCarBase : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTCCarCombatComponent> CombatComponent;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Light;

	UPROPERTY(EditDefaultsOnly)
	USpotLightComponent* HeadLight_R;

	UPROPERTY(EditDefaultsOnly)
	USpotLightComponent* HeadLight_L;
 
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;


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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_Controller() override;


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

	UFUNCTION()
	void DecreaseGas(float InDecreaseGas);

	UFUNCTION()
	void OnRep_UpdateGas();

public:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_UpdateGas)
	float CurrentFuel;
	
	UPROPERTY()
	float MaxFuel;

	bool bLightOn;

	FTimerHandle GasHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gas");
	float MoveFactor;

public:
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return SpringArm; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return Camera; }
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }

	UPROPERTY()
	TObjectPtr<UTCCarWidget> CarWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUISubsystem> UISubsystem;
};
