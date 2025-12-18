// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"
#include "Net/UnrealNetwork.h"
#include <limits>

// Sets default values
AMyAICharacter::AMyAICharacter() :
	WatchedCount(0)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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
		UE_LOG(LogTemp, Error, TEXT("false"))
	}
	else
	{
		SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Error, TEXT("true"))
	}
}