// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_PlayerPanel.generated.h"

class UScrollBox;
class UPlayerInventoryComponent;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_PlayerPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void BindPlayerInventory(UPlayerInventoryComponent* InComp);
	void RefreshInventories();

private:
	UFUNCTION()
	void HandleInventoryReady();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> InventoryScrollBox;

	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> BoundComp;
};
