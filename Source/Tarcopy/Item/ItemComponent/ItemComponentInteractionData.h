#pragma once

#include "CoreMinimal.h"
#include "ItemComponentInteractionData.generated.h"

DECLARE_DELEGATE(FOnInteract)

USTRUCT(BlueprintType)
struct TARCOPY_API FItemComponentInteractionData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FText TextDisplay;
	UPROPERTY()
	uint8 bIsInteractable;

	FOnInteract DelegateInteract;
};
