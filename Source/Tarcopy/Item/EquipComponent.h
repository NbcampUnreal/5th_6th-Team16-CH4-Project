#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipComponent.generated.h"

enum class EEquipSlot : uint32;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API UEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipComponent();

protected:
	virtual void BeginPlay() override;

public:
	FORCEINLINE float GetWeight() const { return TotalWeight; }

protected:
	void EquipItem(int32 ItemId);
	void RemoveItem(int32 ItemId);

	//void UseTool(ToolType);
	
private:
	const struct FItemData* GetItemData(int32 InItemId) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<UDataTable> ItemTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EEquipSlot, int32> EquippedItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TotalWeight;

	static const float WeightMultiplier;
};
