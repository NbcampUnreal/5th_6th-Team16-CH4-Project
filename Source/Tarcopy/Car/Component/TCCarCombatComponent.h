// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Car/Data/TCCarPartDataAsset.h"
#include "TCCarCombatComponent.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API UTCCarCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTCCarCombatComponent();

	void ApplyDamage(UBoxComponent* InBox, float Damage, const FVector& WorldPoint);

protected:
	virtual void BeginPlay() override;

	void DestroyPart(UPrimitiveComponent* DestroyComponent);

	void DestroyWindow(UPrimitiveComponent* DestroyComponent);

	void DestroyWheel(UPrimitiveComponent* DestroyComponent);

	void DestroyMain(UPrimitiveComponent* DestroyComponent);

	void DestroyDefault(UPrimitiveComponent* DestroyComponent);

	void DisableWheelPhysics(UPrimitiveComponent* DestroyComponent);

	UFUNCTION()
	void OnVehicleHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	bool IsPointInsideBox(UBoxComponent* InBox, const FVector& WorldPoint);
	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBox")
	UBoxComponent* FrontBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBox")
	UBoxComponent* BackBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBox")
	UBoxComponent* RightBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBox")
	UBoxComponent* LeftBox;

	UPROPERTY()
	TArray<UBoxComponent*> DamageZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageFactor = 0.00001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDamageImpulse = 50000.f;

	float LastHitTime = 0.f;

	UPROPERTY(EditAnywhere)
	UTCCarPartDataAsset* PartDataAsset;

	UPROPERTY(EditAnywhere)
	TMap<TObjectPtr<UPrimitiveComponent>, FCarPartStat> PartDataMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<TObjectPtr<UPrimitiveComponent>, float> ComponentHealth;

	float DistanceRatio;

	//Test
	UPrimitiveComponent* GetTestMesh();

	UPrimitiveComponent* TestMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStaticMeshComponent*> Meshes;

	
};
