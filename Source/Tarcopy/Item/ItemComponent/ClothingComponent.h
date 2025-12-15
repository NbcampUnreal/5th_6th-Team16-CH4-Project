#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "ClothingComponent.generated.h"

struct FClothData;

UCLASS()
class TARCOPY_API UClothingComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetInteractionDatas(TArray<struct FItemComponentInteractionData>& OutDatas) override;

protected:
	const FClothData* Data;
};
