#pragma once

#include "CoreMinimal.h"
#include "Item/ItemCommand/ItemCommandBase.h"
#include "RepairCommand.generated.h"

UCLASS()
class TARCOPY_API URepairCommand : public UItemCommandBase
{
	GENERATED_BODY()

protected:
	virtual void OnExecute(const FItemCommandContext& Context) override;

public:
	UPROPERTY()
	TWeakObjectPtr<class UDurabilityComponent> OwnerComponent;

	UPROPERTY()
	float Amount;
};
