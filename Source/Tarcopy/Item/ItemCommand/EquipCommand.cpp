#include "Item/ItemCommand/EquipCommand.h"
#include "Item/ItemInstance.h"

void UEquipCommand::OnExecute(const FItemCommandContext& Context)
{
	if (TargetItem.IsValid() == false)
		return;


}
