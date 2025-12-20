#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemComponentBase.generated.h"

class UItemInstance;

UCLASS(Abstract)
class TARCOPY_API UItemComponentBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem);
	UItemInstance* GetOwnerItem() const;

	virtual void GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands) {}

protected:
	const struct FItemData* GetOwnerItemData() const;

protected:
	UPROPERTY()
	TWeakObjectPtr<UItemInstance> OwnerItem;
};
