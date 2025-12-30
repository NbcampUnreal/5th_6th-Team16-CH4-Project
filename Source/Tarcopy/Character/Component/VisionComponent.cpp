// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/VisionComponent.h"
#include "Tarcopy.h"
#include "GameFramework/Character.h"

UVisionComponent::UVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	VisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionMesh"));
	InitSetting();
}

void UVisionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (VisionMesh)
	{
		VisionMesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
		VisionMesh->OnComponentBeginOverlap.AddDynamic(this, &UVisionComponent::OnVisionMeshBeginOverlap);
		VisionMesh->OnComponentEndOverlap.AddDynamic(this, &UVisionComponent::OnVisionMeshEndOverlap);
	}

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (IsValid(Owner))
	{
		if (GetNetMode() == ENetMode::NM_DedicatedServer || Owner->IsLocallyControlled() == false)
		{
			Owner->SetActorHiddenInGame(true);
			VisionMesh->SetVisibility(false);
		}
		else
		{
			Owner->SetActorHiddenInGame(false);
			VisionMesh->SetVisibility(true);
			if (GetWorld())
			{
				GetWorld()->GetTimerManager().SetTimer(VisibilityCheckTimer, this, &UVisionComponent::CheckVisibilityAll, 0.1f, true);
			}
		}
	}
}

void UVisionComponent::OnVisionMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (GetOwnerRole() != ROLE_AutonomousProxy)
	{
		return;
	}
	if (OtherActor->ActorHasTag("InVisible") == false)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (IsValid(Character))
	{
		OverlappedCharacters.Add(Character);
		UE_LOG(LogTemp, Error, TEXT("Overlapped Num : %d"), OverlappedCharacters.Num())
	}
}

void UVisionComponent::OnVisionMeshEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetOwnerRole() != ROLE_AutonomousProxy)
	{
		return;
	}
	if (OtherActor->ActorHasTag("InVisible") == false)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (IsValid(Character))
	{
		OverlappedCharacters.Remove(Character);
		OtherActor->SetActorHiddenInGame(true);
	}
}

void UVisionComponent::InitSetting()
{
	VisionMesh->bRenderInMainPass = false;
	VisionMesh->bRenderInDepthPass = false;
	VisionMesh->bRenderCustomDepth = true;

	VisionMesh->CastShadow = false;
	VisionMesh->bCastDynamicShadow = false;

	VisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	VisionMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	VisionMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	VisionMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_Destructible, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	VisionMesh->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	VisionMesh->SetCollisionResponseToChannel(ECC_PlayerAttack, ECR_Ignore);
}

void UVisionComponent::CheckVisibilityAll()
{
	for (ACharacter* OverlappedActor : OverlappedCharacters)
	{
		FVector MyLocation = GetOwner()->GetActorLocation();
		FVector OtherLocation = OverlappedActor->GetActorLocation();

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		Params.AddIgnoredActor(OverlappedActor);
		Params.bTraceComplex = true;

		bool bHitWall = GetWorld()->LineTraceSingleByChannel(
			Hit,
			MyLocation,
			OtherLocation,
			ECC_Visibility,
			Params
		);

		/*DrawDebugLine(GetWorld(), MyLocation, OtherLocation, FColor::Red, false, 1.0f);
		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			MyLocation,
			OtherLocation,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			{ this },
			EDrawDebugTrace::ForDuration,
			Hit,
			true
		);*/

		if (bHitWall)
		{
			UE_LOG(LogTemp, Error, TEXT("Hit"))
			OverlappedActor->SetActorHiddenInGame(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Not Hit"))
			OverlappedActor->SetActorHiddenInGame(false);
		}
	}
}

