// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_NearbyPanel.generated.h"

class ULootScannerComponent;
class UScrollBox;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUW_NearbyPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void BindScanner(ULootScannerComponent* InScanner);

private:
	void RefreshContainerList();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ContainerScrollBox;

	UPROPERTY()
	TObjectPtr<ULootScannerComponent> BoundScanner;
};
