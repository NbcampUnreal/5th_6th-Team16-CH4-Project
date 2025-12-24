#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "HoldableComponent.generated.h"

enum class EHoldableSocket : uint8;

UCLASS(Abstract)
class TARCOPY_API UHoldableComponent : public UItemComponentBase
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetHolding(bool bInIsHolding);

protected:
	UFUNCTION()
	void OnRep_Holding();

	virtual void SetOwnerHoldingItemMesh() {}

protected:
	void SetOwnerHoldingItemMeshAtSocket(EHoldableSocket Socket);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Holding)
	uint8 bIsHolding : 1 = false;
};
