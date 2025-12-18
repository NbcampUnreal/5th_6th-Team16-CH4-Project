//// Copyright Epic Games, Inc. All Rights Reserved.
//
#include "TCCarBase.h"
//#include "VehicleTestWheelFront.h"
//#include "VehicleTestWheelRear.h"
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

#define LOCTEXT_NAMESPACE "VehiclePawn"

ATCCarBase::ATCCarBase() :
	MaxFuel(100.f),
	MoveFactor(0.12)
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

	CombatComponent = CreateDefaultSubobject<UTCCarCombatComponent>(TEXT("CombatComponent"));

	ChaosVehicleMovement->SetIsReplicated(true);


	//Test
	CurrentFuel = 0.f;


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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATCCarBase::BeginPlay()
{
	Super::BeginPlay();

	//나중에 시동기능을 넣으면 거기로 이동할 예정
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
						float Consumption = 0.02f;
						if (WeakThis->bMovingOnGround)
						{
							float MaxSpeed = WeakThis->GetChaosVehicleMovement()->GetMaxSpeed();
							float CurrentSpeed = WeakThis->GetChaosVehicleMovement()->GetForwardSpeed();
							Consumption += (CurrentSpeed / MaxSpeed) * WeakThis->MoveFactor;
						}
						WeakThis->DecreaseGas(Consumption * 10);
					}),
				1.f,
				true
			);
		}
	}

	//차량 탑승시로 이동예정(UI SubSystem 이용예정)
	auto& Engine = ChaosVehicleMovement->EngineSetup;

	Engine.MaxRPM = 0.1f;
	Engine.EngineIdleRPM = 0.1f;

	ChaosVehicleMovement->RecreatePhysicsState();
	if (IsLocallyControlled())
	{
		if (CarWidgetClass)
		{
			CarWidgetInstance = CreateWidget<UTCCarWidget>(GetWorld(), CarWidgetClass);
			if (CarWidgetInstance)
			{
				CarWidgetInstance->AddToViewport();
				CarWidgetInstance->UpdateFuel(CurrentFuel);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ATCCarController::BeginPlay Cant spawn Widget"));
			}
		}
	}


}

void ATCCarBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATCCarBase::Tick(float Delta)
{
	Super::Tick(Delta);

	bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);


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
}

void ATCCarBase::Steering(const FInputActionValue& Value)
{
	DoSteering(Value.Get<float>());
}

void ATCCarBase::Throttle(const FInputActionValue& Value)
{
	DoThrottle(Value.Get<float>());
}

void ATCCarBase::Brake(const FInputActionValue& Value)
{
	DoBrake(Value.Get<float>());
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
	DoHandLight();
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

void ATCCarBase::DoHandLight()
{
	DamageOn();
	if (bLightOn)
	{
		HeadLight_R->SetVisibility(false);
		HeadLight_L->SetVisibility(false);
		bLightOn = false;
	}
	else
	{
		HeadLight_R->SetVisibility(true);
		HeadLight_L->SetVisibility(true);
		bLightOn = true;
	}
}

void ATCCarBase::DamageOn()
{
	CombatComponent->ApplyDamage(CombatComponent->GetTestMesh(), 100.f);
}

void ATCCarBase::OnRep_UpdateGas()
{
	CarWidgetInstance->UpdateFuel(CurrentFuel);
	UE_LOG(LogTemp, Error, (TEXT("Client Decrease Gas %.0f")), CurrentFuel);
}

void ATCCarBase::DecreaseGas(float InDecreaseGas)
{
	CurrentFuel = FMath::Clamp(CurrentFuel - InDecreaseGas, 0.f, 100.f);
	UE_LOG(LogTemp, Error, (TEXT("Server Decrease Gas %.0f")), CurrentFuel);
}


#undef LOCTEXT_NAMESPACE