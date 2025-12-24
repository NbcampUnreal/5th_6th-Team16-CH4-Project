#include "Common/HealthComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
		
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MaxHP);
	DOREPLIFETIME(ThisClass, CurrentHP);
}

void UHealthComponent::TakeDamage(float Damage, const FName& BoneName)
{
	CurrentHP = FMath::Clamp(CurrentHP - Damage, 0.0f, MaxHP);
	if (CurrentHP <= 0.0f)
	{
		if (OnDead.IsBound() == true)
		{
			OnDead.Broadcast();
		}
	}

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit: %.2f/%.2f"), CurrentHP, MaxHP));
}

