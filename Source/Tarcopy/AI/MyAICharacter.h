// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAICharacter.generated.h"

class UStaticMeshComponent;
class AMyCharacter;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> RootComp;
#pragma endregion

#pragma region Vision

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Viewport", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisionMesh;

#pragma endregion

#pragma region Combat

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsHit;

	int32 AttackDamage;

	UFUNCTION(BlueprintCallable)
	void Attack(AMyAICharacter* ContextActor, AActor* TargetActor);

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void HandleDeath();
#pragma endregion

#pragma region Animation
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AM_Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> AM_Hit;

#pragma endregion
};
