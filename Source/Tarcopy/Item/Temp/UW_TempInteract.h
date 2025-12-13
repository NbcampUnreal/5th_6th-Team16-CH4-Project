#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/ItemComponent/ItemComponentInteractionData.h"
#include "UW_TempInteract.generated.h"

UCLASS()
class TARCOPY_API UUW_TempInteract : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInteract(const FItemComponentInteractionData& InData);

private:
	UFUNCTION()
	void ExecuteInteract();

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextInteract;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BtnInteract;

	FItemComponentInteractionData Data;
};
