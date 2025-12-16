// Fill out your copyright notice in the Description page of Project Settings.


#include "Car/Test/TCCarController.h"
#include "EnhancedInputSubsystems.h"
#include "Car/UI/TCCarWidget.h"
#include "Car/TCCarBase.h"
#include "ChaosWheeledVehicleMovementComponent.h"

void ATCCarController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATCCarController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		if (CarWidgetClass)
		{
			CarWidgetInstance = CreateWidget<UTCCarWidget>(this, CarWidgetClass);
			if (CarWidgetInstance)
			{
				CarWidgetInstance->AddToViewport();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ATCCarController::BeginPlay Cant spawn Widget"));
			}
		}
	}

	PossessedCar = Cast<ATCCarBase>(GetPawn());
	if (PossessedCar)
	{
		PossessedCar->OnFuelChanged.AddDynamic(CarWidgetInstance, &UTCCarWidget::UpdateFuel);
		CarWidgetInstance->UpdateFuel(PossessedCar->CurrentGas);
	}

}

void ATCCarController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(PossessedCar) && IsValid(CarWidgetInstance))
	{
		CarWidgetInstance->UpdateSpeed(PossessedCar->GetChaosVehicleMovement()->GetForwardSpeed());
		CarWidgetInstance->UpdateRPM(PossessedCar->GetChaosVehicleMovement()->GetEngineRotationSpeed());
	}
}
