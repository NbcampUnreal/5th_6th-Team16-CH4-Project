// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TCCarCombatComponent.generated.h"

class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API UTCCarCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTCCarCombatComponent();

	void ApplyDamage(UPrimitiveComponent* HitComp, float Damage);

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	TMap<UPrimitiveComponent*, float> ComponentHealth;

	virtual void BeginPlay() override;

	void DestroyPart(UPrimitiveComponent* DestroyComponent);

	void DestroyWindow(UPrimitiveComponent* DestroyComponent);

	void DestroyWheel(UPrimitiveComponent* DestroyComponent);

	void DestroyMain(UPrimitiveComponent* DestroyComponent);

	void DestroyDefault(UPrimitiveComponent* DestroyComponent);

	void DisableWheelPhysics(int32 WheelIndex);
	
	UFUNCTION()
	void OnVehiclePartHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	int32 FindWheelIndexFromComp(UPrimitiveComponent* DestroyComponent);
	
public:	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> WheelActorClass;

	//Test
	UPrimitiveComponent* GetTestMesh();

	UPrimitiveComponent* TestMesh;

	TArray<UStaticMeshComponent*> Meshes;

	
};
