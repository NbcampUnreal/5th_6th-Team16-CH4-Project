#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemComponent/MeleeWeaponComponent.h"
#include "Item/Data/MeleeWeaponData.h"
#include "Item/DataTableSubsystem.h"
#include "Item/CraftSubsystem.h"
#include "Item/ItemComponent/CraftComponent.h"
#include "Item/ItemComponent/ItemComponentPreset.h"
#include "Item/ItemComponent/DefaultItemComponent.h"

void UItemInstance::SetData(const FItemData* InData)
{
	if (InData == nullptr)
		return;

	Data = InData;

	if (IsValid(Data->ItemComponentPreset) == true)
	{
		for (const auto& Component : Data->ItemComponentPreset->ItemComponentClasses)
		{
			if (IsValid(Component) == false)
				continue;

			UItemComponentBase* NewItemComponent = NewObject<UItemComponentBase>(this, Component);
			NewItemComponent->SetOwnerItem(this);
			ItemComponents.Add(NewItemComponent);
		}
	}

	// CraftTable에서 ItemId가 재료로 사용되면 CraftComponent 자동 생성
	UCraftSubsystem* CraftSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCraftSubsystem>();
	if (IsValid(CraftSubsystem) == true)
	{
		if (CraftSubsystem->GetCraftRecipe(Data->ItemId) != nullptr)
		{
			UCraftComponent* CraftComponent = NewObject<UCraftComponent>(this);
			CraftComponent->SetOwnerItem(this);
			ItemComponents.Add(CraftComponent);
		}
	}

	UItemComponentBase* NewItemComponent = NewObject<UDefaultItemComponent>(this);
	NewItemComponent->SetOwnerItem(this);
	ItemComponents.Add(NewItemComponent);

	// test
	InstanceID = FGuid::NewGuid();
}

const TArray<TObjectPtr<UItemComponentBase>> UItemInstance::GetItemComponents() const
{
	return ItemComponents;
}

void UItemInstance::CancelAllComponentActions()
{
	for (const auto& Component : ItemComponents)
	{
		if (IsValid(Component) == false)
			continue;

		Component->CancelAction();
	}
}
