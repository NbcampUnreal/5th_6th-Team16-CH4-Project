#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoodleComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TARCOPY_API UMoodleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMoodleComponent();

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentHunger() const { return CurrentHunger; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentThirst() const { return CurrentThirst; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentHunger(float InHunger);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentThirst(float InThirst);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentStamina(float InStamina);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMaxStamina(float InStamina);

protected:

	FTimerHandle HungerHandle;
	FTimerHandle ThirstHandle;
	FTimerHandle MaxStaminaHandle;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moodle", meta = (AllowPrivateAccess = "true"))
	float CurrentHunger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moodle", meta = (AllowPrivateAccess = "true"))
	float CurrentThirst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moodle", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moodle", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;
};