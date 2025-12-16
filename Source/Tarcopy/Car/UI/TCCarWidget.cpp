// Fill out your copyright notice in the Description page of Project Settings.


#include "Car/UI/TCCarWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/ProgressBar.h"

void UTCCarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TWeakObjectPtr<UTCCarWidget> WeakThis(this);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SpeedHandler,
			FTimerDelegate::CreateLambda([WeakThis]()
				{
					if (!WeakThis.IsValid())
						return;

					UTCCarWidget* Widget = WeakThis.Get();

					if (!Widget->TextSpeed)
						return;

					Widget->TextSpeed->SetText(
						FText::AsNumber(
							FMath::RoundToInt(Widget->CurrentSpeed)
						)
					);
				}),
			0.2f,
			true
		);
	}

}
void UTCCarWidget::UpdateSpeed(float InValue)
{
	if (!ImageSpeedNeedle) return;

	CurrentSpeed = FMath::Abs(InValue) * 0.036f;

	float Clamped = FMath::Clamp(CurrentSpeed, 0.f, MaxSpeed);
	float Ratio = Clamped / MaxSpeed;
	float Angle = FMath::Lerp(MinAngle, MaxAngle, Ratio);

	ImageSpeedNeedle->SetRenderTransformAngle(Angle);
}

void UTCCarWidget::UpdateRPM(float InValue)
{
	if (!ImageRPMNeedle) return;

	float ClampedRPM = FMath::Clamp(InValue, 0.f, MaxRPM);
	float Ratio = ClampedRPM / MaxRPM;
	float Angle = FMath::Lerp(MinAngle, MaxAngle, Ratio);

	ImageRPMNeedle->SetRenderTransformAngle(Angle);
}

void UTCCarWidget::UpdateFuel(float InValue)
{
	if (!FuelBar) return;

	float ClampedFuel = FMath::Clamp(InValue, 0.f, 100.f);
	
	FuelBar->SetPercent(InValue / 100.f);
}
