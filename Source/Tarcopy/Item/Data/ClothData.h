#pragma once

#include "CoreMinimal.h"
#include "Item/ItemEnums.h"
#include "ClothData.generated.h"

USTRUCT()
struct TARCOPY_API FClothData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBodyLocation BodyLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovementSpeed;									// 이동속도 저하
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackSpeed;										// 공격속도 저하
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxCondition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConditionLoss;									// 데미지 받으면 잃는 내구도
};
