#include "Item/Temp/TempDamageableActor.h"
#include "Common/HealthComponent.h"
#include "Engine/DamageEvents.h"

ATempDamageableActor::ATempDamageableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ATempDamageableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

float ATempDamageableActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (IsValid(HealthComponent) == true)
	{
		const FPointDamageEvent* PointDamageEvent = (const FPointDamageEvent*)(&DamageEvent);
		if (PointDamageEvent != nullptr)
		{
			HealthComponent->TakeDamage(Damage, PointDamageEvent->HitInfo.BoneName);
		}
	}
	return 0.0f;
}
