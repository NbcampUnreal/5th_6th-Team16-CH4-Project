// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAICharacter.generated.h"

UCLASS()
class TARCOPY_API AMyAICharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Character Override
public:
	// Sets default values for this character's properties
	AMyAICharacter();
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
#pragma endregion

#pragma region Vision
public:
	void WatchedCountModify(int32 InIncrement);

protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	int32 WatchedCount;

	UPROPERTY(VisibleAnywhere, Replicated, ReplicatedUsing = OnRep_SetVisible)
	bool bIsVisible;

	UFUNCTION()
	void OnRep_SetVisible();
#pragma endregion


};
