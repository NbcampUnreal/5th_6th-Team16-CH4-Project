// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MyPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include "Item/Temp/UW_TempItem.h"

AMyPlayerController::AMyPlayerController() :
	IMC_Character(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr)
{
	bReplicates = true;
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
		return;

	FInputModeGameAndUI GameAndUI;
	SetInputMode(GameAndUI);
	bShowMouseCursor = true;

	if (IsValid(TempItemClass) == true)
	{
		TempItemInstance = CreateWidget<UUW_TempItem>(this, TempItemClass);
		if (IsValid(TempItemInstance) == true)
		{
			TempItemInstance->AddToViewport(0);
		}
	}
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			checkf(IMC_Character != nullptr, TEXT("IMC_Character is nullptr"));
			Subsystem->AddMappingContext(IMC_Character, 0);
		}
	}
}

void AMyPlayerController::SetItem(UItemInstance* Item)
{
	if (IsValid(TempItemInstance) == false)
		return;

	TempItemInstance->SetItem(Item);
}
