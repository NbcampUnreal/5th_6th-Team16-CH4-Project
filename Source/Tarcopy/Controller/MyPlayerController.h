// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class TARCOPY_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region Controller Override

public:
	AMyPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

#pragma endregion

#pragma region Inputs
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputMappingContext> IMC_Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> WheelAction;

#pragma endregion

};
