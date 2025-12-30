#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "FluidContainerComponent.generated.h"

struct FFluidContainerData;

UCLASS()
class TARCOPY_API UFluidContainerComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_SetComponent() override;

public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_Fill(float InAmount);
	UFUNCTION()
	void OnRep_PrintFluid();

	FORCEINLINE const FFluidContainerData* GetData() { return Data; }
	FORCEINLINE const float GetAmount() { return Amount; }

protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_PrintFluid)
	FName ContainedFluidId;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_PrintFluid)
	float Amount;

	const FFluidContainerData* Data;
};
