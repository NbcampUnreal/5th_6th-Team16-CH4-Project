// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UIConfig.generated.h"

class UUserWidget;

/**
 * 
 */
UENUM()
enum class EUIType : uint8
{
    Title,
    Test
};

UCLASS()
class TARCOPY_API UUIConfig : public UDataAsset
{
	GENERATED_BODY()

public:
    TSubclassOf<UUserWidget> GetWidgetClass(EUIType Type) const;

    UPROPERTY(EditDefaultsOnly)
    TMap<EUIType, TSubclassOf<UUserWidget>> WidgetClasses;
};
