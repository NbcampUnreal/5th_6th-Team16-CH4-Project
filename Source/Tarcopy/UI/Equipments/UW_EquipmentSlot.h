// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_EquipmentSlot.generated.h"

class UItemInstance;
class UImage;
class USizeBox;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_EquipmentSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItem(UItemInstance*);
	void SetSize(FVector2D NewSize);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SlotSize;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img;
};
