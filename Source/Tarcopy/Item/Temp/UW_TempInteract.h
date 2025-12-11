#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TempInteract.generated.h"

UCLASS()
class TARCOPY_API UUW_TempInteract : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInteract(const struct FInteractionData& InData);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextInteract;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BtnInteract;
};
