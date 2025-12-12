
#include "Car/Component/TCCarCombatComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosVehicleWheel.h"
#include "WheeledVehiclePawn.h"
#include "Car/TCCarBase.h"
#include "Car/TCChaosVehicleDummyWheel.h"


UTCCarCombatComponent::UTCCarCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTCCarCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	
	Owner->GetComponents<UStaticMeshComponent>(Meshes);
	
	for (auto Mesh : Meshes)
	{
		ComponentHealth.Add(Mesh, 100);
	}
}

void UTCCarCombatComponent::DestroyPart(UPrimitiveComponent* DestroyComponent)
{
	if (DestroyComponent->ComponentHasTag("Window"))
	{
		DestroyWindow(DestroyComponent);
		return;
	}

	if (DestroyComponent->ComponentHasTag("Wheel"))
	{
		DestroyWheel(DestroyComponent);
		return;
	}

	DestroyComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DestroyComponent->SetSimulatePhysics(true);
	DestroyComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UTCCarCombatComponent::DestroyWindow(UPrimitiveComponent* DestroyComponent)
{
	DestroyComponent->SetVisibility(false);
	DestroyComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//glass effect spawn
}

void UTCCarCombatComponent::DestroyWheel(UPrimitiveComponent* DestroyComponent)
{
	int32 WheelIndex = FindWheelIndexFromComp(DestroyComponent);

	UStaticMeshComponent* WheelMesh = Cast<UStaticMeshComponent>(DestroyComponent);

	WheelMesh->SetVisibility(false);

	DisableWheelPhysics(WheelIndex);
}

void UTCCarCombatComponent::DisableWheelPhysics(int32 WheelIndex)
{
	ATCCarBase* Car = Cast<ATCCarBase>(GetOwner());
	if (!Car) return;

	UChaosWheeledVehicleMovementComponent* Move = Car->GetChaosVehicleMovement();
	if (!Move) return;

	if (!Move->WheelSetups.IsValidIndex(WheelIndex))
		return;

	FChaosWheelSetup& Wheel = Move->WheelSetups[WheelIndex];

	Move->SetWheelFrictionMultiplier(WheelIndex, 0.1f);
	Move->SetWheelHandbrakeTorque(WheelIndex, 0.1f);
	Move->SetWheelMaxBrakeTorque(WheelIndex, 0.f);
	Move->SetWheelMaxSteerAngle(WheelIndex, 0.1f);
	Move->SetWheelRadius(WheelIndex, 10.f);
	Move->SetWheelSlipGraphMultiplier(WheelIndex, 0.1f);

	Move->SetSuspensionParams(0.f, 0.f, 0.f, 0.f, 0.f, WheelIndex);

}

int32 UTCCarCombatComponent::FindWheelIndexFromComp(UPrimitiveComponent* DestroyComponent)
{
	if (!DestroyComponent) return -1;

	AWheeledVehiclePawn* Vehicle = Cast<AWheeledVehiclePawn>(GetOwner());
	if (!Vehicle) return -1;

	USkeletalMeshComponent* Mesh = Vehicle->GetMesh();
	if (!Mesh) return -1;

	FName HitBone = DestroyComponent->GetAttachSocketName();
	if (HitBone.IsNone())
	{
		HitBone = Mesh->FindClosestBone(DestroyComponent->GetComponentLocation());
	}

	ATCCarBase* Car = Cast<ATCCarBase>(GetOwner());
	
	UChaosWheeledVehicleMovementComponent* Move = Car->GetChaosVehicleMovement();
		
	
	if (!Move) return -1;
	for (int32 i = 0; i < Move->WheelSetups.Num(); i++)
	{
		if (Move->WheelSetups[i].BoneName == HitBone)
		{
			UE_LOG(LogTemp, Error, TEXT("Find Index %d"), i);
			return i;		
		}
	}

	return -1;
}

void UTCCarCombatComponent::ApplyDamage(UPrimitiveComponent* HitComp, float Damage)
{
	if (!HitComp) return ;

	if (ComponentHealth.Contains(HitComp))
	{
		ComponentHealth[HitComp] = FMath::Clamp(ComponentHealth[HitComp] - Damage, 0.f, 100.f);

		if (ComponentHealth[HitComp] <= 0)
		{
			DestroyPart(HitComp);
		}
	}
}

void UTCCarCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

UPrimitiveComponent* UTCCarCombatComponent::GetTestMesh()
{
	int32 RandIndex = FMath::RandRange(0, Meshes.Num() - 1);
	UE_LOG(LogTemp, Error, TEXT("%d"), RandIndex);
	UE_LOG(LogTemp, Error, TEXT("%d"), Meshes.Num());
	TestMesh = Meshes[RandIndex];
	return TestMesh;
}

