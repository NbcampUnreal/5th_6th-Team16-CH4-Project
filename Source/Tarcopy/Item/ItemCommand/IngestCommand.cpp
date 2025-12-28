#include "Item/ItemCommand/IngestCommand.h"
#include "Item/ItemComponent/FoodComponent.h"

void UIngestCommand::OnExecute(const FItemCommandContext& Context)
{
	if (OwnerComponent.IsValid() == false)
		return;

	if (Context.Instigator.IsValid() == false)
		return;

	OwnerComponent->ServerRPC_Ingest(EatAmount);
}
