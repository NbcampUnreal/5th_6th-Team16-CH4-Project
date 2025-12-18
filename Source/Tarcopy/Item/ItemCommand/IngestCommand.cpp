#include "Item/ItemCommand/IngestCommand.h"
#include "Item/ItemComponent/FoodComponent.h"
#include "Item/Data/FoodData.h"

void UIngestCommand::OnExecute(const FItemCommandContext& Context)
{
	if (OwnerComponent.IsValid() == false)
		return;

	OwnerComponent->Consume(EatAmount);
}
