#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/WeaponComponent.h"
#include "FirearmComponent.generated.h"

struct FFirearmData;

UCLASS()
class TARCOPY_API UFirearmComponent : public UWeaponComponent
{
	GENERATED_BODY()

public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;

	virtual void SetOwnerHoldingItemMesh() override;

	virtual void OnExecuteAttack() override;

	virtual void CancelAction() override;

protected:
	virtual void OnRep_SetComponent() override;

private:
	void CheckHit(const FVector& StartLocation, const FVector& EndLocation);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayAttackMontage();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_StopAttackMontage();

protected:
	const FFirearmData* Data;
};
