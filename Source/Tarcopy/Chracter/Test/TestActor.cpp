// Fill out your copyright notice in the Description page of Project Settings.


#include "Chracter/Test/TestActor.h"

// Sets default values
ATestActor::ATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ATestActor::Activate(AActor* InInstigator)
{
	UE_LOG(LogTemp, Error, TEXT("Activate"))
}

