#include "Item/ItemCommand/RepairCommand.h"
#include "Item/ItemComponent/DurabilityComponent.h"

void URepairCommand::OnExecute(const FItemCommandContext& Context)
{
	if (OwnerComponent.IsValid() == false)
		return;

	OwnerComponent->RestoreDurability(Amount);
}
