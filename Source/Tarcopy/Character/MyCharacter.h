// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;

UCLASS()
class TARCOPY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Character Override

public:
	AMyCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Viewport Components

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

#pragma endregion

#pragma region Action

	UFUNCTION()
	virtual void MoveAction(const FInputActionValue& Value);


public:
	UPROPERTY(EditAnywhere)
	FName ItemId;

	UFUNCTION()
	void SetItem();
#pragma endregion

};
