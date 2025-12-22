#include "Character/MoodleComponent.h"

UMoodleComponent::UMoodleComponent() :
	CurrentHunger(100.0),
	CurrentThirst(100.0),
	CurrentStamina(100.0),
	MaxStamina(100.0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMoodleComponent::SetCurrentHunger(float InHunger)
{
	CurrentHunger = FMath::Clamp(InHunger, 0.0, 100.0);
}

void UMoodleComponent::SetCurrentThirst(float InThirst)
{
	CurrentThirst = FMath::Clamp(InThirst, 0.0, 100.0);
}

void UMoodleComponent::SetCurrentStamina(float InStamina)
{
	CurrentStamina = FMath::Clamp(InStamina, 0.0, MaxStamina);
}

void UMoodleComponent::SetMaxStamina(float InStamina)
{
	MaxStamina = FMath::Clamp(InStamina, 0.0, 100.0);
}

void UMoodleComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		HungerHandle,
		[this]()
		{
			if (IsValid(this))
				CurrentHunger -= 5.f;
		},
		360.f,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		ThirstHandle,
		[this]()
		{
			if (IsValid(this))
				CurrentThirst -= 5.f;
		},
		300.f,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		MaxStaminaHandle,
		[this]()
		{
			if (IsValid(this))
				MaxStamina -= 1.f;
		},
		300.f,
		true
	);
}
