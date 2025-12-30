#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "FoodComponent.generated.h"

struct FFoodData;

UCLASS()
class TARCOPY_API UFoodComponent : public UItemComponentBase
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
	void ServerRPC_Ingest(int32 ConsumeAmount);
	UFUNCTION()
	void OnRep_PrintAmount();

	FORCEINLINE const FFoodData* GetData() { return Data; }

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_PrintAmount)
	int32 Amount = 4;										// default Amount = 4
	UPROPERTY(VisibleAnywhere, Replicated)
	float RemainToExpire;

	const FFoodData* Data;

	static const float TotalAmount;							// default Amount = 4
};
