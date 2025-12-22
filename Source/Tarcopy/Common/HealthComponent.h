#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDead)

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TARCOPY_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	void TakeDamage(float Damage, const FName& BoneName);

public:
	FOnDead OnDead;

protected:
	UPROPERTY()
	float MaxHP = 100.0f;
	UPROPERTY()
	float CurrentHP = 100.0f;
};
