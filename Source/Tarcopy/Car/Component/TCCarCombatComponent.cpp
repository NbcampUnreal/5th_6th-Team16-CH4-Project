
#include "Car/Component/TCCarCombatComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"


UTCCarCombatComponent::UTCCarCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTCCarCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	TArray<UStaticMeshComponent*> Meshes;
	Owner->GetComponents<UStaticMeshComponent>(Meshes);
	TestMesh = Meshes[0];
	for (auto Mesh : Meshes)
	{
		ComponentHealth.Add(Mesh, 100);
	}
}

void UTCCarCombatComponent::ApplyDamage(UPrimitiveComponent* HitComp, float Damage)
{
	if (!HitComp) return ;

	if (ComponentHealth.Contains(HitComp))
	{
		ComponentHealth[HitComp] = FMath::Clamp(ComponentHealth[HitComp] - Damage, 0.f, 100.f);

		if (ComponentHealth[HitComp] <= 0)
		{
			HitComp->SetVisibility(false);
			HitComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void UTCCarCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

