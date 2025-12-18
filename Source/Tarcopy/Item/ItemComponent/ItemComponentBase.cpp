#include "Item/ItemComponent/ItemComponentBase.h"
#include "Item/ItemInstance.h"

void UItemComponentBase::SetOwnerItem(UItemInstance* InOwnerItem)
{
	OwnerItem = InOwnerItem;
}

const FItemData* UItemComponentBase::GetOwnerItemData() const
{
	return OwnerItem != nullptr ? OwnerItem->GetData() : nullptr;
}