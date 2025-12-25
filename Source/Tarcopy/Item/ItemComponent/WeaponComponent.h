#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "WeaponComponent.generated.h"

UCLASS(Abstract)
class TARCOPY_API UWeaponComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void ExecuteAttack() PURE_VIRTUAL(UWeaponComponent::Attack, );

protected:
	FTimerHandle EnableMovementTimerHandle;

	UPROPERTY(Replicated)
	uint8 bIsAttacking = false;
};
