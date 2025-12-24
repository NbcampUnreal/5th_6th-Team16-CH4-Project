// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"
#include "Net/UnrealNetwork.h"
#include <limits>
#include "AI/ZombieController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyAICharacter::AMyAICharacter() :
	AttackDamage(40)
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

void AMyAICharacter::Attack(AMyAICharacter* ContextActor, AActor* TargetActor)
{
	AMyCharacter* DamagedActor = Cast<AMyCharacter>(TargetActor);
	if (!IsValid(DamagedActor)) return;

	FHitResult Hit;
	FVector StartLocation = ContextActor->GetActorLocation() + FVector({ 0.f, 0.f, 80.f });
	FVector EndLocation = DamagedActor->GetActorLocation() + FMath::FRandRange(0.f, 80.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;

	bool bIsWallHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		Params
	);

	if (!bIsWallHit)
	{
		UGameplayStatics::ApplyPointDamage(DamagedActor, 
											AttackDamage, 
											EndLocation - StartLocation, 
											Hit, 
											GetController(), 
											this, 
											UDamageType::StaticClass());
	}
}