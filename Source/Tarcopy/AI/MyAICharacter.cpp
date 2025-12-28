// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MyAICharacter.h"
#include "Net/UnrealNetwork.h"
#include <limits>
#include "AI/ZombieController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Common/HealthComponent.h"
#include "Components/StateTreeComponent.h"
#include "Car/TCCarBase.h"

// Sets default values
AMyAICharacter::AMyAICharacter() :
	AttackDamage(30),
	bIsAttack(false),
	bIsHit(false)
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
	VisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AMyAICharacter::OnVisionMeshBeginOverlap);
	VisionMesh->OnComponentEndOverlap.AddDynamic(this, &AMyAICharacter::OnVisionMeshEndOverlap);

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	StateTreeComponent->SetStartLogicAutomatically(false);
	//StateTreeComponent->SetIsReplicated(true);
}

void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("InVisible"));
	SetActorHiddenInGame(true);

	if (HasAuthority())
	{
		StateTreeComponent->StartLogic();
	}
	else
	{
		StateTreeComponent->StopLogic("");
	}
}

void AMyAICharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsAttack);
	DOREPLIFETIME(ThisClass, bIsHit);
}

void AMyAICharacter::OnVisionMeshBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (HasAuthority() == false) return;

	AMyCharacter* Player = Cast<AMyCharacter>(OtherActor);
	ATCCarBase* Car = Cast<ATCCarBase>(OtherActor);
	if (IsValid(Player))
	{
		if (Player->IsPlayerControlled() == false) return;
		if (OtherComp == Player->GetCapsuleComponent())
		{
			TargetActor = Player;
			UE_LOG(LogTemp, Warning, TEXT("Target"))
			StateTreeComponent->SendStateTreeEvent(ToChase);
		}
	}
	else if (IsValid(Car))
	{
		if (Car->IsPawnControlled() == false) return;
		if (OtherComp == Car->GetMesh())
		{
			TargetActor = Car;
			StateTreeComponent->SendStateTreeEvent(ToChase);
		}
	}
}

void AMyAICharacter::OnVisionMeshEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (HasAuthority() == false) return;
	if (TargetActor != OtherActor) return;

	AMyCharacter* Player = Cast<AMyCharacter>(OtherActor);
	ATCCarBase* Car = Cast<ATCCarBase>(OtherActor);
	if (IsValid(Player))
	{
		if (Player->IsPlayerControlled() == false) return;
		if (OtherComp == Player->GetCapsuleComponent())
		{
			TargetActor = nullptr;
			StateTreeComponent->SendStateTreeEvent(ToPatrol);
		}
	}
	else if (IsValid(Car))
	{
		if (Car->IsPawnControlled() == false) return;
		if (OtherComp == Car->GetMesh())
		{
			TargetActor = nullptr;
			StateTreeComponent->SendStateTreeEvent(ToPatrol);
		}
	}
}

void AMyAICharacter::OnRep_bIsAttack()
{
	if (bIsAttack)
	{
		PlayAnimMontage(AM_Attack);
	}
	else
	{
		StopAnimMontage(AM_Attack);
	}
}

void AMyAICharacter::OnRep_bIsHit()
{
	if (bIsHit)
	{
		PlayAnimMontage(AM_Hit);
	}
	else
	{
		StopAnimMontage(AM_Hit);
	}
}

void AMyAICharacter::Attack(AMyAICharacter* ContextActor, AActor* DamagedActor)
{
	if (bIsAttack || bIsHit) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		PlayAnimMontage(AM_Attack);
		bIsAttack = true;
		GetCharacterMovement()->DisableMovement();
		FOnMontageEnded AttackMontageEnded;
		AttackMontageEnded.BindUObject(this, &AMyAICharacter::OnAttackMontageEnded);
		AnimInst->Montage_SetEndDelegate(AttackMontageEnded, AM_Attack);
	}

	// Notify로 옮기기
	FHitResult Hit;
	FVector StartLocation = ContextActor->GetActorLocation() + FVector({ 0.f, 0.f, 80.f });
	FVector EndLocation = DamagedActor->GetActorLocation() + FMath::FRandRange(0.f, 80.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;
	Params.bReturnPhysicalMaterial = true;

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

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		PlayAnimMontage(AM_Hit);
		bIsHit = true;
		GetCharacterMovement()->DisableMovement();
		FOnMontageEnded HitMontageEnded;
		HitMontageEnded.BindUObject(this, &AMyAICharacter::OnHitMontageEnded);
		AnimInst->Montage_SetEndDelegate(HitMontageEnded, AM_Hit);
	}

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

void AMyAICharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttack = false;
	if (bInterrupted) return;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyAICharacter::OnHitMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsHit = false;
	if (bInterrupted) return;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
