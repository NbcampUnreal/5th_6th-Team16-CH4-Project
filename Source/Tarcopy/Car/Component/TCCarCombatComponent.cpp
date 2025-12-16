
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

	if (AWheeledVehiclePawn* Vehicle = Cast<AWheeledVehiclePawn>(Owner))
	{
		if (USkeletalMeshComponent* VehicleMesh = Vehicle->GetMesh())
		{
			VehicleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			VehicleMesh->SetNotifyRigidBodyCollision(true);
		}
	}

	Owner->GetComponents<UStaticMeshComponent>(Meshes);
	
	for (auto Mesh : Meshes)
	{
		ComponentHealth.Add(Mesh, 100);

		Mesh->SetSimulatePhysics(false);

		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetNotifyRigidBodyCollision(true);

		Mesh->SetCollisionProfileName(TEXT("CarProfile"));

		Mesh->OnComponentHit.AddDynamic(this, &UTCCarCombatComponent::OnVehiclePartHit);
	}
}

void UTCCarCombatComponent::DestroyPart(UPrimitiveComponent* DestroyComponent)
{
	DestroyComponent->OnComponentHit.RemoveDynamic(this, &UTCCarCombatComponent::OnVehiclePartHit);
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

	if (DestroyComponent->ComponentHasTag("Main"))
	{
		DestroyMain(DestroyComponent);
		return;
	}

	DestroyDefault(DestroyComponent);
	return;
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
	if (!WheelMesh) return;

	if (WheelActorClass) 
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(
			WheelActorClass,
			WheelMesh->GetComponentTransform(),
			Params
		);
	}

	WheelMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	WheelMesh->SetVisibility(false);

	DisableWheelPhysics(WheelIndex);
}

void UTCCarCombatComponent::DestroyMain(UPrimitiveComponent* DestroyComponent)
{
	//자동차 전체파괴
}

void UTCCarCombatComponent::DestroyDefault(UPrimitiveComponent* DestroyComponent)
{
	DestroyComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DestroyComponent->SetSimulatePhysics(true);
	DestroyComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

	/*Move->SetWheelFrictionMultiplier(WheelIndex, 0.1f);
	Move->SetWheelHandbrakeTorque(WheelIndex, 0.1f);
	Move->SetWheelMaxBrakeTorque(WheelIndex, 0.f);
	Move->SetWheelMaxSteerAngle(WheelIndex, 0.1f);
	Move->SetWheelRadius(WheelIndex, 10.f);
	Move->SetWheelSlipGraphMultiplier(WheelIndex, 0.1f);

	Move->SetSuspensionParams(0.f, 0.f, 0.f, 0.f, 0.f, WheelIndex);*///이방법은 너무 연산을 많이먹음 바퀴가 부서지면 차체에서 인풋값을 절반을받거나 하는 방식으로 해야할듯..? 기우는 걸 해결할 방법이 없기는 함.. 이거하니까 차체가 날아감

}

void UTCCarCombatComponent::OnVehiclePartHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor == GetOwner()) return;

	ApplyDamage(HitComp, 100.f);
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

UPrimitiveComponent* UTCCarCombatComponent::GetTestMesh()
{
	int32 RandIndex = FMath::RandRange(0, Meshes.Num() - 1);
	UE_LOG(LogTemp, Error, TEXT("%d"), RandIndex);
	UE_LOG(LogTemp, Error, TEXT("%d"), Meshes.Num());
	TestMesh = Meshes[RandIndex];
	return TestMesh;
}

