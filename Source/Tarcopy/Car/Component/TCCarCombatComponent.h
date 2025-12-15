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

	
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Test
	UPrimitiveComponent* GetTestMesh() { return TestMesh; }

	UPrimitiveComponent* TestMesh;
		
};
