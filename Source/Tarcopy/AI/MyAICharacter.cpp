// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"
#include "Net/UnrealNetwork.h"
#include <limits>
#include "AI/ZombieController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyAICharacter::AMyAICharacter() :
	WatchedCount(0)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	Movement->MaxWalkSpeed = 650.f;
}

void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetActorHiddenInGame(true);
	}
	else
	{
		OnRep_SetVisible();
	}
}

void AMyAICharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsVisible);
	DOREPLIFETIME(ThisClass, WatchedCount);
}

void AMyAICharacter::WatchedCountModify(int32 InIncrement)
{
	if (!HasAuthority()) return;

	WatchedCount = FMath::Clamp(WatchedCount + InIncrement, 0, std::numeric_limits<int32>::max());
	if (WatchedCount > 0)
	{
		bIsVisible = true;
	}
	else
	{
		bIsVisible = false;
	}
}

void AMyAICharacter::OnRep_SetVisible()
{
	if (bIsVisible)
	{
		SetActorHiddenInGame(false);
	}
	else
	{
		SetActorHiddenInGame(true);
	}
}