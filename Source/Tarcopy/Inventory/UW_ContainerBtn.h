// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_ContainerBtn.generated.h"

class UButton;
class UTextBlock;
class UInventoryData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickedWithInventory, UInventoryData* /*Inventory*/);

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_ContainerBtn : public UUserWidget
{
	GENERATED_BODY()
	
public:


protected:
    virtual void NativeConstruct() override;

public:
    void BindInventory(UInventoryData* InInventory, const FText& InDisplayName);

private:
    void RefreshVisual();

    UFUNCTION()
    void HandleClicked();

public:
    FOnClickedWithInventory OnClickedWithInventory;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ContainerBtn;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ContainerName;

    UPROPERTY()
    FText DisplayName;

    UPROPERTY()
    TObjectPtr<UInventoryData> Inventory;
};
