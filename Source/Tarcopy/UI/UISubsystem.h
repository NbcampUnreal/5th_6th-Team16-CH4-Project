// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/UIConfig.h"
#include "UISubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TARCOPY_API UUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UUISubsystem();

	UUserWidget* ShowUI(EUIType Type);
	void HideUI(EUIType Type);

private:
	TObjectPtr<UUIConfig> UIConfigData;

	UPROPERTY()
	TMap<EUIType, TObjectPtr<UUserWidget>> SingleWidgets;
};
