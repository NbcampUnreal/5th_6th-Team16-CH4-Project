#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_TempItem.generated.h"

class UItemInstance;

UCLASS()
class TARCOPY_API UUW_TempItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(UItemInstance* Item);
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextItemId;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUW_TempInteract> InteractUIClass;
	UPROPERTY()
	TArray<TObjectPtr<UUW_TempInteract>> InteractUIs;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UPanelWidget> PanelInteract;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> ItemTable;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> InteractTable;
};
