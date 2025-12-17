#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/WeaponComponent.h"
#include "MeleeWeaponComponent.generated.h"

struct FMeleeWeaponData;

UCLASS()
class TARCOPY_API UMeleeWeaponComponent : public UWeaponComponent
{
	GENERATED_BODY()
	
public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;

	virtual void Attack() override;

protected:
	const FMeleeWeaponData* Data;
};
