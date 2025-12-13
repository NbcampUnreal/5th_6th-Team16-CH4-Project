#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Item/ItemComponent/MeleeWeaponComponent.h"
#include "Item/Data/MeleeWeaponData.h"
#include "Item/DataTableSubsystem.h"
#include "Item/CraftSubsystem.h"
#include "Item/ItemComponent/CraftComponent.h"

void UItemInstance::SetData(const FItemData* InData)
{
	if (InData == nullptr)
		return;

	Data = InData;

	for (const auto& Component : Data->ItemComponents)
	{
		if (IsValid(Component) == false)
			continue;

		UItemComponentBase* NewItemComponent = NewObject<UItemComponentBase>(this, Component);
		NewItemComponent->SetOwnerItem(this);
		ItemComponents.Add(NewItemComponent);
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
}

const TArray<TObjectPtr<UItemComponentBase>> UItemInstance::GetItemComponents() const
{
	return ItemComponents;
}
