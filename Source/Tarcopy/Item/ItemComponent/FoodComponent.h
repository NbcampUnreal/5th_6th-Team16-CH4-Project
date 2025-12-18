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

	void Consume(int32 ConsumeAmount);

private:
	UPROPERTY(VisibleAnywhere)
	int32 Amount = 4;										// default Amount = 4
	UPROPERTY(VisibleAnywhere)
	float RemainToExpire;

	const FFoodData* Data;
};
