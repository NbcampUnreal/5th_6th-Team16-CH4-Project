// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controller/MyPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Item/EquipComponent.h"
#include "Item/ItemInstance.h"
#include "Framework/DoorInteractComponent.h"
#include "Framework/DoorTagUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MoodleComponent.h"
#include "AI/MyAICharacter.h"
#include "Character/ActivateInterface.h"

// Sets default values
AMyCharacter::AMyCharacter() :
	BaseWalkSpeed(600.f),
	SprintSpeedMultiplier(1.5f),
	CrouchSpeedMultiplier(0.7f),
	bIsAttackMode(false)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = 1800.f;
	SpringArm->SetRelativeRotation(FRotator(-50.f, 45.f, 0.f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;

	// Create the camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	VisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisionMesh"));
	VisionMesh->SetupAttachment(RootComponent);
	VisionMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	VisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnVisionMeshBeginOverlap);
	VisionMesh->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnVisionMeshEndOverlap);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(200.f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMyCharacter::OnInteractionSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AMyCharacter::OnInteractionSphereEndOverlap);

	Moodle = CreateDefaultSubobject<UMoodleComponent>(TEXT("Moodle"));
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastObstructionTrace += DeltaTime;
	if (TimeSinceLastObstructionTrace >= ObstructionTraceInterval)
	{
		TimeSinceLastObstructionTrace = 0.f;
		UpdateCameraObstructionFade();
	}
}

void AMyCharacter::UpdateCameraObstructionFade()
{
	if (!IsValid(Camera) || !IsValid(GetWorld()))
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();

	// Unhide components whose hold time expired.
	for (auto It = FadeHoldUntil.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid() || Now > It.Value())
		{
			if (It.Key().IsValid())
			{
				It.Key()->SetVisibility(true, true);
			}
			It.RemoveCurrent();
		}
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = GetActorLocation() + FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	FCollisionQueryParams Params(SCENE_QUERY_STAT(CameraOcclusion), /*bTraceComplex=*/ false);
	Params.AddIgnoredActor(this);

	TArray<FHitResult> Hits;
	if (GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Visibility, Params))
	{
		for (const FHitResult& Hit : Hits)
		{
			UPrimitiveComponent* HitComp = Hit.GetComponent();
			if (!IsValid(HitComp))
			{
				continue;
			}

			// Skip overlaps with our own components.
			if (HitComp->GetOwner() == this)
			{
				continue;
			}

			// Refresh hold time and hide if not already hidden.
			FadeHoldUntil.FindOrAdd(HitComp) = Now + FadeHoldTime;
			if (HitComp->IsVisible())
			{
				HitComp->SetVisibility(false, true);
			}
		}
	}
}

void AMyCharacter::OnVisionMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	if (HasAuthority() == false)
		return;

	AMyAICharacter* MyAI = Cast<AMyAICharacter>(OtherActor);
	if (IsValid(MyAI) == false)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Overlap"));

	FVector MyLocation = GetActorLocation();
	FVector OtherLocation = OtherActor->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OtherActor);
	Params.bTraceComplex = true;

	bool bHitWall = GetWorld()->LineTraceSingleByChannel(
		Hit,
		MyLocation,
		OtherLocation,
		ECC_WorldStatic,
		Params
	);

	if (!bHitWall)
	{
		MyAI->WatchedCountModify(1);
	}
}

void AMyCharacter::OnVisionMeshEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority() == false)
		return;

	AMyAICharacter* MyAI = Cast<AMyAICharacter>(OtherActor);
	if (IsValid(MyAI) == false)
		return;

	MyAI->WatchedCountModify(-1);
}

void AMyCharacter::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorHasDoorTagOrDoorMesh(OtherActor))
	{
		AddInteractableDoor(OtherActor);
	}
}

void AMyCharacter::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor))
	{
		RemoveInteractableDoor(OtherActor);
	}
}

void AMyCharacter::MoveAction(const FInputActionValue& Value)
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is Invalid."));
		return;
	}

	const FVector2D InMovementVector = Value.Get<FVector2D>();

	AddMovementInput(Camera->GetForwardVector(), InMovementVector.X);
	AddMovementInput(Camera->GetRightVector(), InMovementVector.Y);
}

void AMyCharacter::StartSprint(const FInputActionValue& Value)
{
	if (bIsAttackMode)
		return;

	if (GetCharacterMovement()->MaxWalkSpeed != BaseWalkSpeed * SprintSpeedMultiplier)
	{
		ServerRPC_SetSpeed(BaseWalkSpeed * SprintSpeedMultiplier);
	}
	if (!bIsCrouched)
	{
		SpringArm->TargetOffset.Z = 100.f;
	}
}

void AMyCharacter::ServerRPC_SetSpeed_Implementation(float InSpeed)
{
	if (GetCharacterMovement())
	{
		CurrentSpeed = InSpeed;
		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	}
}

void AMyCharacter::StopSprint(const FInputActionValue& Value)
{
	SpringArm->TargetOffset.Z = 0.f;

	if (bIsAttackMode)
		return;

	ServerRPC_SetSpeed(BaseWalkSpeed);
	if (!bIsCrouched)
	{
		SpringArm->TargetOffset.Z = 0.f;
	}
}

void AMyCharacter::OnRep_SetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void AMyCharacter::StartCrouch(const FInputActionValue& Value)
{
	if (bIsAttackMode)
		return;

	ServerRPC_Crouch();
}

void AMyCharacter::ServerRPC_Crouch_Implementation()
{
	MulticastRPC_Crouch();
}

void AMyCharacter::MulticastRPC_Crouch_Implementation()
{
	if (GetCharacterMovement())
	{
		if (bIsCrouched)
		{
			GetCharacterMovement()->MaxWalkSpeedCrouched = BaseWalkSpeed;
			UnCrouch(true);
		}
		else
		{
			if (CanCrouch())
			{
				GetCharacterMovement()->MaxWalkSpeedCrouched = BaseWalkSpeed * CrouchSpeedMultiplier;
				Crouch(true);
			}
		}
	}
}

void AMyCharacter::Wheel(const FInputActionValue& Value)
{
	if (!IsLocallyControlled())
		return;

	const float Input = Value.Get<float>() * -200;
	SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + Input, 900.f, 1800.f);
}

void AMyCharacter::CanceledRightClick(const FInputActionValue& Value)
{
	bIsAttackMode = false;

	AMyPlayerController* MyPC = GetOwner<AMyPlayerController>();
	if (!IsValid(MyPC))
		return;

	FVector WorldLocation, WorldDirection;
	if (MyPC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection) == false)
		return;

	FVector Start = Camera->GetComponentLocation();
	FVector Direction = WorldLocation - Start;
	FVector End = Start + Direction * 10000.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Line Trace Error"));
		return;
	}

	if (AActor* HitActor = Hit.GetActor())
	{
		if (IActivateInterface* Activatable = Cast<IActivateInterface>(HitActor))
		{
			Activatable->Activate(this);  
			return;
		}

		TInlineComponentArray<UActorComponent*> Components(HitActor);
		for (UActorComponent* Component : Components)
		{
			if (IActivateInterface* ActivatableComponent = Cast<IActivateInterface>(Component))
			{
				ActivatableComponent->Activate(this);
				return;
			}
		}
	}
}

void AMyCharacter::TriggeredRightClick(const FInputActionValue& Value)
{
	TurnToMouse();
	SpringArm->TargetOffset.Z = 0.f;

	if (bIsAttackMode)
		return;

	bIsAttackMode = true;
	ServerRPC_SetSpeed(BaseWalkSpeed * CrouchSpeedMultiplier);
}

void AMyCharacter::CompletedRightClick(const FInputActionValue& Value)
{
	SpringArm->TargetOffset.Z = 0.f;
	if (bIsAttackMode)
	{
		ServerRPC_StopTurnToMouse();
		ServerRPC_SetSpeed(BaseWalkSpeed);
		bIsAttackMode = false;
	}
}

void AMyCharacter::LeftClick(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Left Click"))
}

void AMyCharacter::ServerRPC_TurnToMouse_Implementation(const FRotator& TargetRot)
{
	MulticastRPC_TurnToMouse(TargetRot);
}

void AMyCharacter::MulticastRPC_TurnToMouse_Implementation(const FRotator& TargetRot)
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorRotation(TargetRot);
}

void AMyCharacter::TurnToMouse()
{
	AMyPlayerController* MyPC = GetOwner<AMyPlayerController>();
	if (!IsValid(MyPC))
		return;

	FVector WorldLocation, WorldDirection;
	if (MyPC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection) == false)
		return;

	FVector Start = Camera->GetComponentLocation();
	FVector Direction = WorldLocation - Start;
	FVector End = Start + Direction * 10000.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Line Trace Error"));
		return;
	}
	FVector TargetPoint = Hit.ImpactPoint;
	FRotator TargetRot = {0.f, (TargetPoint - GetActorLocation()).Rotation().Yaw, 0.f};
	ServerRPC_TurnToMouse(TargetRot);
}

void AMyCharacter::MulticastRPC_StopTurnToMouse_Implementation()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AMyCharacter::ServerRPC_StopTurnToMouse_Implementation()
{
	MulticastRPC_StopTurnToMouse();
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this,
										  &AMyCharacter::MoveAction);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this,
										  &AMyCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this,
										  &AMyCharacter::StopSprint);
			}
			if (PlayerController->CrouchAction)
			{
				EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Started, this,
										  &AMyCharacter::StartCrouch);
			}
			if (PlayerController->WheelAction)
			{
				EnhancedInput->BindAction(PlayerController->WheelAction, ETriggerEvent::Started, this,
										  &AMyCharacter::Wheel);
			}
			if (PlayerController->RightClickAction)
			{
				EnhancedInput->BindAction(PlayerController->RightClickAction, ETriggerEvent::Canceled, this,
										  &AMyCharacter::CanceledRightClick);
				EnhancedInput->BindAction(PlayerController->RightClickAction, ETriggerEvent::Triggered, this,
										  &AMyCharacter::TriggeredRightClick);
				EnhancedInput->BindAction(PlayerController->RightClickAction, ETriggerEvent::Completed, this,
										  &AMyCharacter::CompletedRightClick);
			}

			if (PlayerController->LeftClickAction)
			{
				EnhancedInput->BindAction(PlayerController->LeftClickAction, ETriggerEvent::Started, this,
					&AMyCharacter::LeftClick);
			}

			// 아이템 테스트용
			if (PlayerController->ItemAction)
			{
				EnhancedInput->BindAction(PlayerController->ItemAction, ETriggerEvent::Started, this,
											&AMyCharacter::SetItem);
			}
			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(PlayerController->InteractAction, ETriggerEvent::Started, this,
											&AMyCharacter::Interact);
			}
		}
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BaseWalkSpeed);
	DOREPLIFETIME(ThisClass, SprintSpeedMultiplier);
	DOREPLIFETIME(ThisClass, CrouchSpeedMultiplier);
	DOREPLIFETIME(ThisClass, CurrentSpeed);
	DOREPLIFETIME(ThisClass, bIsAttackMode);
}

void AMyCharacter::SetItem()
{
	if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
	{
		UEquipComponent* EquipComponent = FindComponentByClass<UEquipComponent>();
		if (IsValid(EquipComponent) == false)
			return;

		const auto& EquippedItems = EquipComponent->GetEquippedItems();
		for (const auto& Pair : EquippedItems)
		{
			if (IsValid(Pair.Value) == true)
			{
				PlayerController->SetItem(Pair.Value);
				break;
			}
		}
	}
}

void AMyCharacter::AddInteractableDoor(AActor* DoorActor)
{
	if (ActorHasDoorTagOrDoorMesh(DoorActor))
	{
		OverlappingDoors.Add(DoorActor);
	}
}

void AMyCharacter::RemoveInteractableDoor(AActor* DoorActor)
{
	if (IsValid(DoorActor))
	{
		OverlappingDoors.Remove(DoorActor);
	}
}

float AMyCharacter::GetCurrentHunger() 
{
	return  Moodle->GetCurrentHunger(); 
}
float AMyCharacter::GetCurrentThirst()
{
	return Moodle->GetCurrentThirst();
}
float AMyCharacter::GetCurrentStamina()
{
	return Moodle->GetCurrentStamina();
}
float AMyCharacter::GetMaxStamina()
{
	return Moodle->GetMaxStamina();
}

void AMyCharacter::SetCurrentHunger(float InHunger)
{
	Moodle->SetCurrentHunger(InHunger);
}
void AMyCharacter::SetCurrentThirst(float InThirst)
{
	Moodle->SetCurrentThirst(InThirst);
}

void AMyCharacter::SetCurrentStamina(float InStamina)
{
	Moodle->SetCurrentStamina(InStamina);
}

void AMyCharacter::SetMaxStamina(float InStamina)
{
	Moodle->SetMaxStamina(InStamina);
}

void AMyCharacter::Interact(const FInputActionValue& Value)
{
	if (!IsLocallyControlled() || !IsValid(InteractionSphere))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Interact key pressed. Overlapping door count: %d"), OverlappingDoors.Num());

	float ClosestDistSq = TNumericLimits<float>::Max();
	AActor* ClosestDoor = nullptr;

	for (const TWeakObjectPtr<AActor>& DoorPtr : OverlappingDoors)
	{
		if (!DoorPtr.IsValid())
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(DoorPtr->GetActorLocation(), GetActorLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestDoor = DoorPtr.Get();
		}
	}

	if (IsValid(ClosestDoor))
	{
		UE_LOG(LogTemp, Log, TEXT("Interact door found: %s"), *ClosestDoor->GetName());
		ServerRPC_ToggleDoor(ClosestDoor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact failed: no door in range."));
	}
}

void AMyCharacter::ServerRPC_ToggleDoor_Implementation(AActor* DoorActor)
{
	if (!ActorHasDoorTagOrDoorMesh(DoorActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerRPC_ToggleDoor aborted: invalid door actor."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<AActor*> AffectedDoors;
	AffectedDoors.Reserve(2);

	static const FString DoorGroupPrefix(TEXT("DoorGroup_"));
	FName GroupTag = NAME_None;
	for (const FName& Tag : DoorActor->Tags)
	{
		if (Tag.ToString().StartsWith(DoorGroupPrefix))
		{
			GroupTag = Tag;
			break;
		}
	}

	if (GroupTag == NAME_None)
	{
		AffectedDoors.Add(DoorActor);
	}
	else
	{
		TArray<AActor*> GroupActors;
		UGameplayStatics::GetAllActorsWithTag(World, GroupTag, GroupActors);
		for (AActor* GroupDoorActor : GroupActors)
		{
			if (ActorHasDoorTagOrDoorMesh(GroupDoorActor))
			{
				AffectedDoors.AddUnique(GroupDoorActor);
			}
		}

		if (AffectedDoors.IsEmpty())
		{
			AffectedDoors.Add(DoorActor);
		}
	}

	if (IActivateInterface* Activatable = Cast<IActivateInterface>(DoorActor))
	{
		Activatable->Activate(this);
	}
	else if (UDoorInteractComponent* DoorComp = DoorActor->FindComponentByClass<UDoorInteractComponent>())
	{
		UE_LOG(LogTemp, Log, TEXT("Activating existing door component on %s"), *DoorActor->GetName());
		DoorComp->Activate(this);
	}
	else
	{
		UDoorInteractComponent* NewComp = NewObject<UDoorInteractComponent>(DoorActor);
		if (IsValid(NewComp))
		{
			NewComp->RegisterComponent();
			UE_LOG(LogTemp, Log, TEXT("Created door component and activating %s"), *DoorActor->GetName());
			NewComp->Activate(this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create door component on %s"), *DoorActor->GetName());
			return;
		}
	}

	TArray<FTransform> DoorTransforms;
	DoorTransforms.Reserve(AffectedDoors.Num());
	for (AActor* AffectedDoor : AffectedDoors)
	{
		if (!IsValid(AffectedDoor))
		{
			continue;
		}
		DoorTransforms.Add(AffectedDoor->GetActorTransform());
	}

	MulticastRPC_ApplyDoorTransforms(AffectedDoors, DoorTransforms);
}

void AMyCharacter::MulticastRPC_ApplyDoorTransforms_Implementation(const TArray<AActor*>& DoorActors, const TArray<FTransform>& DoorTransforms)
{
	const int32 Count = FMath::Min(DoorActors.Num(), DoorTransforms.Num());
	for (int32 i = 0; i < Count; ++i)
	{
		AActor* DoorActor = DoorActors[i];
		if (!IsValid(DoorActor))
		{
			continue;
		}

		if (UStaticMeshComponent* DoorMeshComp = Cast<UStaticMeshComponent>(DoorActor->GetRootComponent()))
		{
			if (DoorMeshComp->Mobility != EComponentMobility::Movable)
			{
				DoorMeshComp->SetMobility(EComponentMobility::Movable);
			}
		}

		DoorActor->SetActorTransform(DoorTransforms[i]);
	}
}
