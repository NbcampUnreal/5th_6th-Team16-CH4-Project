#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "FluidContainerComponent.generated.h"

struct FFluidContainerData;

UCLASS()
class TARCOPY_API UFluidContainerComponent : public UItemComponentBase
{
	GENERATED_BODY()

public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ContainedFluidId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Amount;

	const FFluidContainerData* Data;
};
