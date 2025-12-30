// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/VisionComponent.h"
#include "Tarcopy.h"
#include "GameFramework/Character.h"
#include "Car/TCCarBase.h"
#include "Kismet/KismetSystemLibrary.h"

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

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	ATCCarBase* Car = Cast<ATCCarBase>(GetOwner());
	if (IsValid(Character))
	{
		if (Character->IsLocallyControlled())
		{
			Character->SetActorHiddenInGame(false);
			VisionMesh->SetVisibility(true, true);
		}
		else
		{
			Character->SetActorHiddenInGame(true);
			VisionMesh->SetVisibility(false, true);
		}
	}
	else if (IsValid(Car))
	{
		Car->SetActorHiddenInGame(false);
		VisionMesh->SetVisibility(false, true);
	}


	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(VisibilityCheckTimer, this, &UVisionComponent::CheckVisibilityAll, 0.1f, true);
	}

	//if (GetNetMode() == ENetMode::NM_DedicatedServer || IsLocallyControlled() == false)
	//{
	//	SetActorHiddenInGame(true);
	//	VisionComponent->SetVisibility(false, true);
	//}
}

void UVisionComponent::OnVisionMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (GetOwnerRole() == ROLE_Authority)
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
	}
}

void UVisionComponent::OnVisionMeshEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		return;
	}
	if (OtherActor->ActorHasTag("InVisible") == false)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (IsValid(Character) && OverlappedCharacters.Contains(Character))
	{
		OverlappedCharacters.Remove(Character);
		if (Cast<APawn>(GetOwner())->IsLocallyControlled())
		{
			OtherActor->SetActorHiddenInGame(true);
		}
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
	if (Cast<APawn>(GetOwner())->IsLocallyControlled() && IsVisible() && VisionMesh->IsVisible())
	{
		UE_LOG(LogTemp, Error, TEXT("Overlapped Num : %d"), OverlappedCharacters.Num())
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

			DrawDebugLine(GetWorld(), MyLocation, OtherLocation, FColor::Red, false, 0.1f);
			UKismetSystemLibrary::LineTraceSingle(
				GetWorld(),
				MyLocation,
				OtherLocation,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				true,
				{ GetOwner(), OverlappedActor },
				EDrawDebugTrace::ForDuration,
				Hit,
				true,
				FLinearColor::Red,
				FLinearColor::Green,
				0.1f
			);

			if (bHitWall)
			{
				OverlappedActor->SetActorHiddenInGame(true);
			}
			else
			{
				OverlappedActor->SetActorHiddenInGame(false);
			}
		}
	}
	
}

