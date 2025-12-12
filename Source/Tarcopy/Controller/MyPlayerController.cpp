// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MyPlayerController.h"
#include <EnhancedInputSubsystems.h>

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
