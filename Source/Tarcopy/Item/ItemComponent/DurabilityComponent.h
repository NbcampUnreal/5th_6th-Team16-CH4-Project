#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "DurabilityComponent.generated.h"

struct FDurabilityData;

UCLASS()
class TARCOPY_API UDurabilityComponent : public UItemComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_SetComponent() override;

public:
	void LoseDurability(float Amount);
	UFUNCTION(Server, Reliable)
	void ServerRPC_RestoreDurability(float Amount);

	FORCEINLINE const FDurabilityData* GetData() { return Data; }
	FORCEINLINE const float GetCondition() { return Condition; }

protected:
	UFUNCTION()
	virtual void OnRep_PrintCondition();

protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_PrintCondition)
	float Condition;

	const FDurabilityData* Data;
};
