// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TCCarController.generated.h"

class UTCCarWidget;
class UInputMappingContext;
class ATCCarBase;

UCLASS()
class TARCOPY_API ATCCarController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	virtual void Tick(float Delta) override;

public:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UTCCarWidget> CarWidgetClass;

	UPROPERTY()
	TObjectPtr<UTCCarWidget> CarWidgetInstance;

	UPROPERTY()
	ATCCarBase* PossessedCar;

	

};
