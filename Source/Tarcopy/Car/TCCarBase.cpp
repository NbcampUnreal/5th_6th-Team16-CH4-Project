//// Copyright Epic Games, Inc. All Rights Reserved.
//
#include "TCCarBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/SpotLightComponent.h"
#include "Component/TCCarCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Car/UI/TCCarWidget.h"
#include "UI/UISubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/MyPlayerController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Car/TCVehicleWheelFront.h"
#include "Car/TCVehicleWheelRear.h"
#include "Engine/DamageEvents.h"
#include "Car/UI/TCCarActivate.h"


#define LOCTEXT_NAMESPACE "VehiclePawn"

ATCCarBase::ATCCarBase() :
	MaxFuel(100.f),
	MoveFactor(0.12),
	RidePawn(nullptr),
	bPossessed(false),
	bLightOn(false)
{
	bReplicates = true;
	// construct the front camera boom
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 2500.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetUsingAbsoluteRotation(false);
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("CarProfile"));

	Light = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass"));
	Light->SetupAttachment(GetRootComponent());

	HeadLight_R = CreateDefaultSubobject<USpotLightComponent>(TEXT("PointLight_R"));
	HeadLight_R->SetupAttachment(Light, FName("HeadLight_R"));
	HeadLight_R->SetVisibility(false);
	HeadLight_R->SetIntensity(120000.f);
	HeadLight_R->SetAttenuationRadius(4000.f);
	HeadLight_L = CreateDefaultSubobject<USpotLightComponent>(TEXT("PointLight_L"));
	HeadLight_L->SetupAttachment(Light, FName("HeadLight_L"));
	HeadLight_L->SetVisibility(false);
	HeadLight_L->SetIntensity(120000.f);
	HeadLight_L->SetAttenuationRadius(4000.f);

	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	ChaosVehicleMovement->SetIsReplicated(true);

	CombatComponent = CreateDefaultSubobject<UTCCarCombatComponent>(TEXT("CombatComponent"));
	if (CombatComponent)
	{
		for (auto& Zone : CombatComponent->DamageZone)
		{
			Zone->SetupAttachment(GetMesh());
		}
	}

	GetMesh()->SetAngularDamping(5.0f);
	//Test
	CurrentFuel = 100.f;

	AIControllerClass = nullptr;
	AutoPossessAI = EAutoPossessAI::Disabled;

	NetCullDistanceSquared = FMath::Square(15000.f);
}

void ATCCarBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ATCCarBase::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ATCCarBase::Steering);


		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ATCCarBase::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ATCCarBase::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ATCCarBase::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ATCCarBase::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ATCCarBase::StopBrake);


		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ATCCarBase::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ATCCarBase::StopHandbrake);

		// Light
		EnhancedInputComponent->BindAction(LightAction, ETriggerEvent::Started, this, &ATCCarBase::ToggleLight);

		EnhancedInputComponent->BindAction(InterAction, ETriggerEvent::Started, this, &ATCCarBase::StartInterAction);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATCCarBase::BeginPlay()
{
	Super::BeginPlay();
}

void ATCCarBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATCCarBase::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsLocallyControlled() && IsValid(CarWidgetInstance))
	{
		CarWidgetInstance->UpdateSpeed(ChaosVehicleMovement->GetForwardSpeed());
		CarWidgetInstance->UpdateRPM(ChaosVehicleMovement->GetEngineRotationSpeed());
	}
}

void ATCCarBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentFuel);
	DOREPLIFETIME(ThisClass, RidePawn);
	DOREPLIFETIME(ThisClass, bPossessed);
	DOREPLIFETIME(ThisClass, SteeringFactor);
	DOREPLIFETIME(ThisClass, ThrottleFactor);
	DOREPLIFETIME(ThisClass, bLightOn);
}

void ATCCarBase::UnPossessed()
{
	Super::UnPossessed();

	UE_LOG(LogTemp, Error, TEXT("UnPossessed"));
}

float ATCCarBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority()) return 0;


	if (!DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		return 0;
	}

	const FPointDamageEvent* PointEvent =
		static_cast<const FPointDamageEvent*>(&DamageEvent);
	if (!PointEvent) return 0;

	FVector WorldPoint = PointEvent->HitInfo.ImpactPoint;
	if (CombatComponent)
	{
		for (auto Zone : CombatComponent->DamageZone)
		{
			if (CombatComponent->IsPointInsideBox(Zone, WorldPoint))
			{
				CombatComponent->ApplyDamage(Zone, DamageAmount, WorldPoint);
			}
		}
	}
	
	return DamageAmount;
}

void ATCCarBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	TWeakObjectPtr<ATCCarBase> WeakThis(this);

	if (HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				GasHandler,
				FTimerDelegate::CreateLambda([WeakThis]()
					{
						if (!WeakThis.IsValid())
							return;

						float Consumption = 0.f;

						float CurrentRPM = WeakThis->ChaosVehicleMovement->GetEngineRotationSpeed();
						float ClampedRate = FMath::Clamp(CurrentRPM / 6000.f, 0.f, 1.f);
						Consumption += ClampedRate * WeakThis->MoveFactor;

						WeakThis->DecreaseGas(Consumption * 10);
					}),
				1.f,
				true
			);
		}
	}
}

void ATCCarBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (!IsLocallyControlled()) return;
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
	if (!PC)
	{
		if (CarWidgetInstance)
		{
			UISubsystem->HideUI(EUIType::Car);
		}
		return;
	}

	ULocalPlayer* LP = PC->GetLocalPlayer();
	checkf(LP, TEXT("ATCCarBase::OnRep_Controller() LP"));

	UISubsystem = LP->GetSubsystem<UUISubsystem>();
	if (IsValid(UISubsystem))
	{
		CarWidgetInstance = Cast<UTCCarWidget>(UISubsystem->ShowUI(EUIType::Car));
	}

	ACharacter* PlayerCharacter = Cast<ACharacter>(RidePawn);
	ServerRPCHideCharacter(PlayerCharacter);

	PC->ChangeIMC(PC->IMC_Car);
}


void ATCCarBase::Steering(const FInputActionValue& Value)
{
	DoSteering(Value.Get<float>() * SteeringFactor);
}

void ATCCarBase::Throttle(const FInputActionValue& Value)
{
	DoThrottle(Value.Get<float>() * ThrottleFactor);
}

void ATCCarBase::Brake(const FInputActionValue& Value)
{
	DoBrake(Value.Get<float>() * ThrottleFactor);
}

void ATCCarBase::StartBrake(const FInputActionValue& Value)
{
	DoBrakeStart();
}

void ATCCarBase::StopBrake(const FInputActionValue& Value)
{
	DoBrakeStop();
}

void ATCCarBase::StartHandbrake(const FInputActionValue& Value)
{
	DoHandbrakeStart();
}

void ATCCarBase::StopHandbrake(const FInputActionValue& Value)
{
	DoHandbrakeStop();
}

void ATCCarBase::ToggleLight(const FInputActionValue& Value)
{
	ServerRPCDoHandLight();
}

void ATCCarBase::StartInterAction(const FInputActionValue& Value)
{
	Activate(this);
}

void ATCCarBase::DoSteering(float SteeringValue)
{
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ATCCarBase::DoThrottle(float ThrottleValue)
{
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);

	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ATCCarBase::DoBrake(float BrakeValue)
{
	ChaosVehicleMovement->SetBrakeInput(BrakeValue);

	ChaosVehicleMovement->SetThrottleInput(0.0f);
}

void ATCCarBase::DoBrakeStart()
{
	BrakeLights(true);
}

void ATCCarBase::DoBrakeStop()
{
	BrakeLights(false);

	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ATCCarBase::DoHandbrakeStart()
{
	ChaosVehicleMovement->SetHandbrakeInput(true);

	BrakeLights(true);
}

void ATCCarBase::DoHandbrakeStop()
{
	ChaosVehicleMovement->SetHandbrakeInput(false);

	BrakeLights(false);
}

void ATCCarBase::ServerRPCDoHandLight_Implementation()
{
	if (bLightOn)
	{
		bLightOn = false;
	}
	else
	{
		bLightOn = true;
	}
}

void ATCCarBase::OnRep_UpdateGas()
{
	if (!IsLocallyControlled()) return;
	CarWidgetInstance->UpdateFuel(CurrentFuel);
}

void ATCCarBase::OnRep_bLightOn()
{
	if (bLightOn)
	{
		HeadLight_R->SetVisibility(false);
		HeadLight_L->SetVisibility(false);
	}
	else
	{
		HeadLight_R->SetVisibility(true);
		HeadLight_L->SetVisibility(true);
	}
}

void ATCCarBase::EnterVehicle(APawn* InPawn, APlayerController* InPC)
{
	AMyPlayerController* PC = Cast<AMyPlayerController>(InPC);
	if (!PC) return;

	ACharacter* PlayerCharacter = Cast<ACharacter>(InPawn);
	checkf(PlayerCharacter, TEXT("No Character"));

	RidePawn = PlayerCharacter;

	PC->ServerRPCChangePossess(this);
}

void ATCCarBase::ExitVehicle(APawn* InPawn, APlayerController* InPC)
{
	ServerRPCShowCharacter();
}

void ATCCarBase::ServerRPCAddFuel_Implementation()
{
	CurrentFuel = FMath::Clamp(CurrentFuel + 30.f, 0.f, MaxFuel);
}

void ATCCarBase::DecreaseGas(float InDecreaseGas)
{
	CurrentFuel = FMath::Clamp(CurrentFuel - InDecreaseGas, 0.f, 100.f);
}

void ATCCarBase::Activate(AActor* InInstigator)
{
	
	APawn* Pawn = Cast<APawn>(InInstigator);
	if (!Pawn) return;

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return;

	ShowInterActionUI(PC);

	/*if (InInstigator == this)
	{
		ExitVehicle(Pawn, PC);
		return;
	}
	EnterVehicle(Pawn, PC);*/
}

bool ATCCarBase::FindDismountLocation(FVector& OutLocation) const
{
	APawn* Pawn = RidePawn;
	if (!Pawn) return false;

	ACharacter* Character = Cast<ACharacter>(Pawn);
	if (!Character) return false;

	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	if (!Capsule) return false;

	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	const FVector CarLocation = GetActorLocation();
	const FRotator CarRotation = GetActorRotation();

	TArray<FVector> Directions = {
	-CarRotation.RotateVector(FVector::RightVector),
	 CarRotation.RotateVector(FVector::RightVector)/*,
	-CarRotation.RotateVector(FVector::ForwardVector)*/
	};

	UWorld* World = GetWorld();
	if (!World) return false;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Character);

	for (const FVector& Dir : Directions)
	{
		const FVector Candidate = CarLocation + Dir * (CapsuleRadius + 200.f);

		const FVector SweepStart = Candidate + FVector(0, 0, CapsuleHalfHeight + 50.f);
		const FVector SweepEnd = SweepStart;

		FHitResult Hit;
		bool bBlocked = World->SweepSingleByChannel(
			Hit,
			SweepStart,
			SweepEnd,
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
			Params
		);

		if (bBlocked) continue;

		const FVector FloorStart = Candidate + FVector(0, 0, 50.f);
		const FVector FloorEnd = Candidate - FVector(0, 0, 300.f);

		FHitResult FloorHit;
		bool bHasFloor = World->LineTraceSingleByChannel(
			FloorHit,
			FloorStart,
			FloorEnd,
			ECC_Visibility,
			Params
		);

		if (!bHasFloor) continue;

		OutLocation = FloorHit.ImpactPoint + FVector(0, 0, CapsuleHalfHeight);
		return true;
	}

	return false;
}

void ATCCarBase::DisableWheel(UPrimitiveComponent* DestroyComponent)
{
	if (DestroyComponent->ComponentHasTag("Front"))
	{
		SteeringFactor -= 0.5;
		ThrottleFactor -= 0.15;
	}
	else if (DestroyComponent->ComponentHasTag("Back"))
	{
		ThrottleFactor -= 0.35;
	}
}

void ATCCarBase::ExecuteCommand(ECarCommand Command, APawn* InPawn, APlayerController* InPC)
{
	switch (Command)
	{
		case ECarCommand::Enter : 
			EnterVehicle(InPawn, InPC);
			break;
		
		case ECarCommand::Exit :
			ExitVehicle(InPawn, InPC);
			break;
	}
}

TArray<ECarCommand> ATCCarBase::GetAvailableCommands() const
{
	TArray<ECarCommand> Commands;

	Commands.Add(ECarCommand::Enter);
	Commands.Add(ECarCommand::Exit);

	return Commands;
}

void ATCCarBase::ShowInterActionUI(APlayerController* InPC)
{
	if (!InPC || !InterActionClass) return;

	float MouseX, MouseY;
	InPC->GetMousePosition(MouseX, MouseY);

	InterActionWidget = CreateWidget<UTCCarActivate>(InPC, InterActionClass);

	InterActionWidget->Setup(this);
	InterActionWidget->AddToViewport();
	InterActionWidget->SetKeyboardFocus();
	InterActionWidget->SetPositionInViewport(FVector2D(MouseX, MouseY));

}

void ATCCarBase::ServerRPCShowCharacter_Implementation()
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(RidePawn);
	UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();

	FVector OutLocation = FVector::ZeroVector;
	FRotator OutRotation = GetActorRotation();
	bool bCanDismount = FindDismountLocation(OutLocation);
	if (!bCanDismount) return;
	PlayerCharacter->TeleportTo(OutLocation, OutRotation);

	bPossessed = false;

	Capsule->SetCollisionEnabled(TempSaveCollision);

	PlayerCharacter->SetActorHiddenInGame(false);

	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	AMyPlayerController* PC = Cast<AMyPlayerController>(GetController());
	PC->ServerRPCChangePossess(RidePawn);
}

void ATCCarBase::ServerRPCHideCharacter_Implementation(ACharacter* InCharacter)
{
	RidePawn = InCharacter;

	bPossessed = true;

	UCapsuleComponent* Capsule = InCharacter->GetCapsuleComponent();
	TempSaveCollision = Capsule->GetCollisionEnabled();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InCharacter->SetActorHiddenInGame(true);

	InCharacter->GetCharacterMovement()->DisableMovement();
}


void ATCCarBase::OnRep_bPossessed()
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(RidePawn);
	if (!PlayerCharacter) return;
	if (bPossessed)
	{

		UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();

		TempSaveCollision = Capsule->GetCollisionEnabled();

		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		UCapsuleComponent* Capsule = PlayerCharacter->GetCapsuleComponent();

		Capsule->SetCollisionEnabled(TempSaveCollision);
	}
}
#undef LOCTEXT_NAMESPACE