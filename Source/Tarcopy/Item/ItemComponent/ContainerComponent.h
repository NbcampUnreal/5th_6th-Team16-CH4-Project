#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "ContainerComponent.generated.h"

struct FContainerData;

UCLASS()
class TARCOPY_API UContainerComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;

private:
	const FContainerData* Data;
};
