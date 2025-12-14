#pragma once

#include "CoreMinimal.h"
#include "Item/ItemComponent/ItemComponentBase.h"
#include "MeleeWeaponComponent.generated.h"

struct FMeleeWeaponData;

UCLASS()
class TARCOPY_API UMeleeWeaponComponent : public UItemComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void SetOwnerItem(UItemInstance* InOwnerItem) override;
	virtual void GetInteractionDatas(TArray<struct FItemComponentInteractionData>& OutDatas) override;

protected:
	UPROPERTY(VisibleAnywhere)
	float Condition;

	const FMeleeWeaponData* Data;
};
