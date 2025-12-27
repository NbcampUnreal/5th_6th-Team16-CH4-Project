// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"
#include "Net/UnrealNetwork.h"
#include <limits>
#include "AI/ZombieController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "Common/HealthComponent.h"

// Sets default values
AMyAICharacter::AMyAICharacter() :
	AttackDamage(40),
	bIsAttack(false),
	bIsHit(false)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AIControllerClass = AZombieController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;
	GetCapsuleComponent()->SetupAttachment(RootComponent);

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
	if (bIsAttack || bIsHit) return;

	bIsAttack = true;	
	PlayAnimMontage(AM_Attack);
	bIsAttack = false;

	// Notify로 옮기기
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

float AMyAICharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority()) return Damage;

	Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	bIsHit = true;
	PlayAnimMontage(AM_Hit);
	bIsHit = false;

	if (IsValid(HealthComponent) == true)
	{
		const FPointDamageEvent* PointDamageEvent = (const FPointDamageEvent*)(&DamageEvent);
		if (PointDamageEvent != nullptr)
		{
			Damage = HealthComponent->TakeDamage(Damage, PointDamageEvent->HitInfo);
		}
	}

	return Damage;
}

void AMyAICharacter::HandleDeath()
{
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	// 레그돌, 지면에만 충돌, 다른 물체와는 no collision, 약한 참조자로 참조하고 1분뒤에 제거
	//GetCapsuleComponent()->Setcollision
}
