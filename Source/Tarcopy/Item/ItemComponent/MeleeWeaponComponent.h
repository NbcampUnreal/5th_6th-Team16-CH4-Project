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

	virtual void ExecuteAttack() override;

	virtual void CancelAction() override;

protected:
	virtual void OnRep_SetComponent() override;

private:
	void CheckHit();
	bool CheckIsAttackableTarget(AActor* TargetActor);

	void EnableOwnerMovement();

protected:
	UPROPERTY()
	TSet<AActor*> HitActors;

	FTimerHandle CheckHitTimerHandle;

	const static float CheckHitDelay;						// default: 근접 공격 애니메이션 시간 = 1초, 공격 시점 = 0.6초

	const FMeleeWeaponData* Data;
};
