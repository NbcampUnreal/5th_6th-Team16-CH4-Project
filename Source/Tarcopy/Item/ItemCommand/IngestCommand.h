#pragma once

#include "CoreMinimal.h"
#include "Item/ItemCommand/ItemCommandBase.h"
#include "IngestCommand.generated.h"

UCLASS()
class TARCOPY_API UIngestCommand : public UItemCommandBase
{
	GENERATED_BODY()

protected:
	virtual void OnExecute(const FItemCommandContext& Context) override;

public:
	UPROPERTY()
	TWeakObjectPtr<class UFoodComponent> OwnerComponent;

	UPROPERTY()
	int32 EatAmount;							// 모든 음식의 전체 양 = 4
};
