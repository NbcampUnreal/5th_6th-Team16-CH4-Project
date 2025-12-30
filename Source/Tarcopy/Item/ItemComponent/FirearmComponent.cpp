#include "Item/ItemComponent/FirearmComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/FirearmData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "Character/MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Tarcopy.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemCommand/EquipCommand.h"
#include "Item/ItemComponent/DurabilityComponent.h"
#include "Item/EquipComponent.h"
#include "Inventory/PlayerInventoryComponent.h"
#include "Inventory/InventoryData.h"

const float UFirearmComponent::PerfectShotMultiplier = 1.5f;

void UFirearmComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	if (Data == nullptr)
		return;
}

void UFirearmComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands, const struct FItemCommandContext& Context)
{
	Super::GetCommands(OutCommands, Context);

	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	if (Context.Instigator.IsValid() == false)
		return;

	UEquipComponent* EquipComponent = Context.Instigator->FindComponentByClass<UEquipComponent>();
	UPlayerInventoryComponent* InventoryComponent = Context.Instigator->FindComponentByClass<UPlayerInventoryComponent>();
	if (IsValid(EquipComponent) == false || IsValid(InventoryComponent) == false)
		return;

	bool bIsEquipped = IsValid(GetOwnerCharacter()) == true;
	UEquipCommand* EquipCommand = NewObject<UEquipCommand>(this);
	EquipCommand->TargetItem = GetOwnerItem();
	EquipCommand->TextDisplay = FText::Format(
		FText::FromString(bIsEquipped == true ? TEXT("Unequip {0}") : TEXT("Equip {0}")),
		TextItemName);
	EquipCommand->bEquip = !bIsEquipped;
	EquipCommand->BodyLocation = Data->BodyLocation;
	// 인벤토리에서 공간 있는지 체크해야 함
	UInventoryData* InventoryData = InventoryComponent->GetPlayerInventoryData();
	FIntPoint Origin;
	bool bRotated;
	if (bIsEquipped == true)
	{
		EquipCommand->bExecutable = IsValid(InventoryData) == true && InventoryData->CanAddItem(OwnerItem.Get(), Origin, bRotated) == true;
	}
	else
	{
		TArray<UItemInstance*> ReplaceItems;
		EquipComponent->GetNeedToReplace(Data->BodyLocation, ReplaceItems);
		if (ReplaceItems.Num() == 1)
		{
			EquipCommand->bExecutable = IsValid(InventoryData) == true && InventoryData->CanAddItem(OwnerItem.Get(), Origin, bRotated, ReplaceItems[0]) == true;
		}
		else
		{
			EquipCommand->bExecutable = true;
		}
	}
	OutCommands.Add(EquipCommand);
}

void UFirearmComponent::SetOwnerHoldingItemMesh()
{
	if (Data == nullptr)
		return;

	SetOwnerHoldingItemMeshAtSocket(Data->Socket);
}

void UFirearmComponent::SetOwnerAnimPreset()
{
	if (Data == nullptr)
		return;

	SetOwnerAnimPresetByHoldableType(Data->HoldableType);
}

void UFirearmComponent::OnExecuteAttack(const FVector& TargetLocation)
{
	if (Data == nullptr)
		return;

	AMyCharacter* MyCharacter = Cast<AMyCharacter>(GetOwnerCharacter());
	if (IsValid(MyCharacter) == false)
		return;

	UCharacterMovementComponent* CharacterMovement = MyCharacter->GetCharacterMovement();
	if (IsValid(CharacterMovement) == false)
		return;

	bIsAttacking = true;

	NetMulticast_PlayAttackMontage();

	float AttackDuration = IsValid(Data->Montage) == true ? Data->Montage->GetPlayLength() : 1.0f;
	CharacterMovement->DisableMovement();
	MyCharacter->GetWorldTimerManager().SetTimer(
		EnableMovementTimerHandle,
		this,
		&ThisClass::EnableOwnerMovement,
		AttackDuration,
		false);

	FVector StartLocation = MyCharacter->GetActorLocation();		// 액터 Muzzle 소켓으로 바꿔야 함
	FVector TargetDir = (TargetLocation - StartLocation).GetSafeNormal();
	FVector EndLocation = StartLocation + TargetDir * Data->MaxRange;
	CheckHit(StartLocation, EndLocation);

	UDurabilityComponent* DurabilityComponent = GetOwnerItem()->GetItemComponent<UDurabilityComponent>();
	if (IsValid(DurabilityComponent) == true)
	{
		DurabilityComponent->LoseDurability(Data->LoseCondition);
	}
}

void UFirearmComponent::CancelAction()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == true)
	{
		World->GetTimerManager().ClearTimer(EnableMovementTimerHandle);
	}

	NetMulticast_StopAttackMontage();
	EnableOwnerMovement();
}

void UFirearmComponent::OnRep_SetComponent()
{
	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::FirearmTable)->FindRow<FFirearmData>(ItemData->ItemId, FString(""));
}

void UFirearmComponent::CheckHit(const FVector& StartLocation, const FVector& EndLocation)
{
	if (Data == nullptr)
		return;

	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.bTraceComplex = true;
	Params.bReturnPhysicalMaterial = true;
	// 적 캡슐 Visibility 끄고 나머진 켜기
	FCollisionShape SphereCollision = FCollisionShape::MakeSphere(10.0f);
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECC_PlayerAttack, SphereCollision, Params);
	FVector ActualEndLocation = bHit == true ? HitResult.ImpactPoint : EndLocation;

	// 나이아가라 트레일 연출

	float MinRangeSquared = FMath::Square(Data->MinRange);
	float MaxRangeSquared = FMath::Square(Data->MaxRange);

	if (bHit == true)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) == false)
			return;

		float DistSquared = FVector::DistSquared(StartLocation, ActualEndLocation);
		// 기본 데미지는 MinDamage ~ MaxDamage 랜덤값
		float Damage = FMath::RandRange(Data->MinDamage, Data->MaxDamage);
		// 총기류는 사거리의 끝(최대 사거리의 0.9 이상에서 맞으면 퍼펙트 사격으로 최대 데미지)
		if (DistSquared >= MaxRangeSquared * 0.81f)
		{
			Damage *= PerfectShotMultiplier;
		}

		FVector Dir = (ActualEndLocation - StartLocation).GetSafeNormal();
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			Damage,
			Dir,
			HitResult,
			OwnerCharacter->GetController(),
			OwnerCharacter,
			nullptr);
	}

	// 충돌 검사 디버그
	{
		FVector TraceVec = ActualEndLocation - StartLocation;
		float TraceLen = TraceVec.Size();
		FVector Center = StartLocation + TraceVec * 0.5f;
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();

		DrawDebugCapsule(
			GetWorld(),
			Center,
			(TraceLen * 0.5f) + 10.0f,
			10.0f,
			CapsuleRot,
			bHit == true ? FColor::Green : FColor::Red,
			false,
			5.0f);
	}
}

void UFirearmComponent::NetMulticast_PlayAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	if (IsValid(Data->Montage) == false)
		return;

	OwnerCharacter->PlayAnimMontage(Data->Montage);
}

void UFirearmComponent::NetMulticast_StopAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	if (IsValid(Data->Montage) == false)
		return;

	OwnerCharacter->StopAnimMontage(Data->Montage);
}
