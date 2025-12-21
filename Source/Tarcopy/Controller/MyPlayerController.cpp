// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MyPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include "Item/Temp/UW_TempItem.h"

AMyPlayerController::AMyPlayerController() :
	IMC_Character(nullptr),
	IMC_Car(nullptr),
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
	GameAndUI.SetHideCursorDuringCapture(false).SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
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
			CurrentIMC = IMC_Character;
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

void AMyPlayerController::ChangeIMC(UInputMappingContext* InIMC)
{
	if (!IsLocalPlayerController() || !InIMC) return;

	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP) return;

	auto* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem) return;

	Subsystem->RemoveMappingContext(CurrentIMC);

	Subsystem->AddMappingContext(InIMC, 0);
	CurrentIMC = InIMC;
}

void AMyPlayerController::ServerRPCChangePossess_Implementation(APawn* NewPawn)
{
	OnPossess(NewPawn);
}
