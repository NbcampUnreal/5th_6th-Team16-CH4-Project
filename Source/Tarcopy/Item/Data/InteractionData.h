#pragma once

#include "CoreMinimal.h"
#include "Item/ItemEnums.h"
#include "InteractionData.generated.h"

USTRUCT(BlueprintType)
struct TARCOPY_API FInteractionData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 InteractId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInteractType InteractionType;							// UI 출력용
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UInteractionTask> InteractionTaskClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequiredToolId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ToolDurabilityCost;								// 소모되는 도구 내구도
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int32, int32> IngredientItems;						// 재료 아이템(Id, 갯수) 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int32, int32> GainedItems;							// 획득 아이템 (결과)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TextInteract;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDuration = 1.0f;

};
