#include "Item/ItemComponent/MeleeWeaponComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/MeleeWeaponData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

const float UMeleeWeaponComponent::DamageDelay = 0.6f;

void UMeleeWeaponComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MeleeWeaponTable)->FindRow<FMeleeWeaponData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;
}

void UMeleeWeaponComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	Super::GetCommands(OutCommands);
}

void UMeleeWeaponComponent::ExecuteAttack(ACharacter* OwnerCharacter)
{
	if (IsValid(OwnerCharacter) == false)
		return;

	UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement();
	if (IsValid(CharacterMovement) == false)
		return;

	CharacterMovement->DisableMovement();
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		DamageTimerHandle,
		[WeakThis = TWeakObjectPtr(this), OwnerCharacter]
		{
			if (WeakThis.IsValid() == false)
				return;

			WeakThis->CheckHit(OwnerCharacter);
		},
		DamageDelay,
		false);
}

void UMeleeWeaponComponent::CheckHit(ACharacter* OwnerCharacter)
{
	if (IsValid(OwnerCharacter) == false)
		return;

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Check Hit"));
}
