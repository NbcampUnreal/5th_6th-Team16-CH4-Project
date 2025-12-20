#include "Item/ItemComponent/ItemComponentBase.h"
#include "Item/ItemInstance.h"

void UItemComponentBase::SetOwnerItem(UItemInstance* InOwnerItem)
{
	OwnerItem = InOwnerItem;
}

UItemInstance* UItemComponentBase::GetOwnerItem() const
{
	return OwnerItem.IsValid() == true ? OwnerItem.Get() : nullptr;
}

const FItemData* UItemComponentBase::GetOwnerItemData() const
{
	return OwnerItem.IsValid() == true ? OwnerItem->GetData() : nullptr;
}