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

	virtual void ExecuteAttack(ACharacter* OwnerCharacter) override;

private:
	void CheckHit(ACharacter* OwnerCharacter);

protected:
	FTimerHandle DamageTimerHandle;

	const FMeleeWeaponData* Data;

	const static float DamageDelay;						// default: 근접 공격 애니메이션 시간 = 1초, 공격 시점 = 0.6초
};
