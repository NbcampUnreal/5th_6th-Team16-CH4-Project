// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/ActivateInterface.h"
#include "TestActor.generated.h"

class IActivateInterface;

UCLASS()
class TARCOPY_API ATestActor : public AActor, public IActivateInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestActor();

	virtual void Activate(AActor* InInstigator) override;

};
