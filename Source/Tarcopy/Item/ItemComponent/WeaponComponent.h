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
	
	virtual void Attack() PURE_VIRTUAL(UWeaponComponent::Attack, );
};
