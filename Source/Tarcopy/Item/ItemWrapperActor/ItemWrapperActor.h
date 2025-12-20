#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemWrapperActor.generated.h"

class UItemInstance;

UCLASS()
class TARCOPY_API AItemWrapperActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AItemWrapperActor();

protected:
	virtual void BeginPlay() override;

public:
	void SetItemInstance(UItemInstance* InItemInstance);
	FORCEINLINE UItemInstance* GetItemInstance() const { return ItemInstance; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USphereComponent> LootSphere;						// 아이템 감지용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> DefaultMesh;

	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance;
};
