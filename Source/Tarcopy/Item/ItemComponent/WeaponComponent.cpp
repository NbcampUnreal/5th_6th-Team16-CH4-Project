#include "Item/ItemComponent/WeaponComponent.h"
#include "Net/UnrealNetwork.h"

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsAttacking);
}
