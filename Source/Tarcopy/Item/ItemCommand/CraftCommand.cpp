#include "Item/ItemCommand/CraftCommand.h"
#include "Item/ItemComponent/CraftComponent.h"

void UCraftCommand::OnExecute(const FItemCommandContext& Context)
{
	if (OwnerComponent.IsValid() == false)
		return;

	OwnerComponent->ServereRPC_Craft(CraftTargetId);
}
