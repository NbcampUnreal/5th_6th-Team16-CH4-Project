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

	void LoseDurability(float Amount);
	void RestoreDurability(float Amount);

protected:
	UPROPERTY(VisibleAnywhere)
	float Condition;

	const FDurabilityData* Data;
};
