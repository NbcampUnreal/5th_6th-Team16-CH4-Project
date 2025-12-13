#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipComponent.generated.h"

enum class EBodyLocation : uint32;
class UItemInstance;

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
	const TMap<EBodyLocation, TObjectPtr<UItemInstance>>& GetEquippedItems() const { return EquippedItems; }

protected:
	void EquipItem(EBodyLocation BodyLocation, UItemInstance* Item);
	void RemoveItem(UItemInstance* Item);
	//void UseTool(ToolType);
	
private:
	const struct FItemData* GetItemData(const FName& InItemId) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDataTable> ItemTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<EBodyLocation, TObjectPtr<UItemInstance>> EquippedItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TotalWeight;

	static const float WeightMultiplier;
};
