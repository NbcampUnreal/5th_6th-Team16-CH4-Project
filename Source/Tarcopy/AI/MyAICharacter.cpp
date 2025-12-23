// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"
#include "Net/UnrealNetwork.h"
#include <limits>
#include "AI/ZombieController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyAICharacter::AMyAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	Movement->MaxWalkSpeed = 300.f;

	VisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionMesh"));
	VisionMesh->SetupAttachment(RootComponent);
	VisionMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("InVisible"));
	SetActorHiddenInGame(true);
}

void AMyAICharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}