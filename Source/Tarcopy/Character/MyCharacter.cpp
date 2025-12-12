// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controller/MyPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyCharacter::AMyCharacter() :
	BaseWalkSpeed(600.f),
	SprintSpeedMultiplier(1.5f),
	CrouchSpeedMultiplier(0.7f)
{
	PrimaryActorTick.bCanEverTick = false;
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
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

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
	if (GetCharacterMovement())
	{
		ServerRPC_StartSprint();
		if (!bIsCrouched)
		{
			SpringArm->TargetOffset.Z += 100.f;
		}
	}
}

void AMyCharacter::ServerRPC_StartSprint_Implementation()
{
	if (GetCharacterMovement())
	{
		CurrentSpeed = BaseWalkSpeed * SprintSpeedMultiplier;
		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	}
}

void AMyCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		ServerRPC_StopSprint();
		if (!bIsCrouched)
		{
			SpringArm->TargetOffset.Z -= 100.f;
		}
	}
}

void AMyCharacter::ServerRPC_StopSprint_Implementation()
{
	if (GetCharacterMovement())
	{
		CurrentSpeed = BaseWalkSpeed;
		GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
	}
}

void AMyCharacter::OnRep_SetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void AMyCharacter::StartCrouch(const FInputActionValue& Value)
{
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
				EnhancedInput->BindAction(PlayerController->MoveAction,	ETriggerEvent::Triggered, this, &AMyCharacter::MoveAction);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Started, this, &AMyCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
			}
			if (PlayerController->CrouchAction)
			{
				EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Started, this, &AMyCharacter::StartCrouch);
			}
			if (PlayerController->WheelAction)
			{
				EnhancedInput->BindAction(PlayerController->WheelAction, ETriggerEvent::Started, this, &AMyCharacter::Wheel);
			}
		}
	}
}

void AMyCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentSpeed);
}

