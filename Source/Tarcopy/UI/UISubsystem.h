// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/UIConfig.h"
#include "UISubsystem.generated.h"

class UUW_RootHUD;
class UCanvasPanelSlot;
class UInventoryData;
class UUW_InventoryBorder;
struct FGuid;

/**
 * 
 */
UCLASS()
class TARCOPY_API UUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UUISubsystem();

	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;

	UUserWidget* ShowUI(EUIType Type);
	void HideUI(EUIType Type);

	UUW_InventoryBorder* ShowInventoryUI(UInventoryData* InventoryData);
	void HideInventoryUI(FGuid InventoryID);

private:
	void InitRootHUD();

	void ApplyLayoutPreset(UCanvasPanelSlot* Slot, const FUILayoutPreset& Layout);

	UPROPERTY()
	TObjectPtr<UUW_RootHUD> RootHUD;

	UPROPERTY()
	TObjectPtr<UUIConfig> UIConfigData;

	UPROPERTY()
	TMap<EUIType, TObjectPtr<UUserWidget>> SingleWidgets;

	UPROPERTY()
	TMap<FGuid, TObjectPtr<UUW_InventoryBorder>> InventoryWidgets;
};
