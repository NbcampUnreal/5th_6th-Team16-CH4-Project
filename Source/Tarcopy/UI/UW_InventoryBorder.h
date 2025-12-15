// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_InventoryBorder.generated.h"

class UTextBlock;
class UButton;
class UNamedSlot;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_InventoryBorder : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetContentWidget(UWidget* InWidget);
	
private:
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UTextBlock> ContainerName;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UButton> CloseBtn;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UNamedSlot> ContentSlot;
	
};
