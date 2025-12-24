#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/ItemEnums.h"
#include "EquipComponent.generated.h"

class UItemInstance;

USTRUCT()
struct FEquippedItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	EBodyLocation Location = EBodyLocation::None;
	UPROPERTY()
	TObjectPtr<UItemInstance> Item;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TARCOPY_API UEquipComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipComponent();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

public:
	FORCEINLINE float GetWeight() const { return TotalWeight; }
	FORCEINLINE const TArray<FEquippedItemInfo>& GetEquippedItemInfos() const { return EquippedItemInfos; }
	UItemInstance* GetEquippedItem(EBodyLocation Bodylocation) const;

public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipItem(EBodyLocation BodyLocation, UItemInstance* Item);
	void EquipItem(EBodyLocation BodyLocation, UItemInstance* Item);
	UFUNCTION(Server, Reliable)
	void ServerRPC_UnequipItem(UItemInstance* Item);
	void UnequipItem(UItemInstance* Item);

	void ExecuteAttack();
	void CancelActions();
	
private:
	const struct FItemData* GetItemData(const FName& InItemId) const;

protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<FEquippedItemInfo> EquippedItemInfos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TotalWeight;

	// For Test
	UPROPERTY(EditAnywhere)
	FName TestEquippedItem = FName(TEXT("Axe1"));

	static const float WeightMultiplier;
};
