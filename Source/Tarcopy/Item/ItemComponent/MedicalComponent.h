#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "MedicalComponent.generated.h"

struct FMedicalData;

UCLASS()
class TARCOPY_API UMedicalComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands, const struct FItemCommandContext& Context) override;

protected:
	virtual void OnRep_SetComponent() override;

	virtual void OnExecuteAction(AActor* InInstigator, const struct FItemNetworkContext& NetworkContext) override;

public:
	void RestoreHealth(AActor* InInstigator);

	FORCEINLINE const FMedicalData* GetData() { return Data; }

protected:
	const FMedicalData* Data;
};
