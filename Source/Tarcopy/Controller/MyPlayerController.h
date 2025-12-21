// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UUW_TempItem;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Car")
	TObjectPtr<UInputMappingContext> IMC_Car;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> RightClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs|Character")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> ItemAction;

#pragma endregion

public:
	void SetItem(class UItemInstance* Item);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUW_TempItem> TempItemClass;
	UPROPERTY()
	TObjectPtr<UUW_TempItem> TempItemInstance;

public:
	void ChangeIMC(UInputMappingContext* InIMC);

	TObjectPtr<UInputMappingContext> CurrentIMC;

	UFUNCTION(Server,Reliable)
	void ServerRPCChangePossess(APawn* NewPawn);
};
