#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TempItem.generated.h"

UCLASS()
class TARCOPY_API UUW_TempItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(int32 ItemId);
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextItemId;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUW_TempInteract> InteractUIClass;
	UPROPERTY()
	TArray<TObjectPtr<UUW_TempInteract>> InteractUIs;
};
