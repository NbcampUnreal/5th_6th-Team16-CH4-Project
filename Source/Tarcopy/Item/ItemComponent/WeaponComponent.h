#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "WeaponComponent.generated.h"

UCLASS(Abstract)
class TARCOPY_API UWeaponComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;
	
	virtual void ExecuteAttack(ACharacter* OwnerCharacter) PURE_VIRTUAL(UWeaponComponent::Attack, );

protected:
	UPROPERTY()
	TWeakObjectPtr<ACharacter> CachedOwner;

	FTimerHandle EnableMovementTimerHandle;

	UPROPERTY()
	uint8 bIsAttacking = false;
};
