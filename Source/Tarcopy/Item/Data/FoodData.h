#pragma once

#include "CoreMinimal.h"
#include "FoodData.generated.h"

USTRUCT()
struct TARCOPY_API FFoodData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Hunger;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Thirst;
};
