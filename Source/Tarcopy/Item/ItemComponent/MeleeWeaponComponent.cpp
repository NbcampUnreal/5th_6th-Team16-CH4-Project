#include "Item/ItemComponent/MeleeWeaponComponent.h"
#include "Item/DataTableSubsystem.h"
#include "Item/Data/MeleeWeaponData.h"
#include "Item/ItemInstance.h"
#include "Item/Data/ItemData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "FirearmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item/ItemCommand/EquipCommand.h"
#include "Item/ItemComponent/DurabilityComponent.h"
#include "Character/MyCharacter.h"
#include "HoldableComponent.h"

const float UMeleeWeaponComponent::CheckHitDelay = 0.5f;

void UMeleeWeaponComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);
}

void UMeleeWeaponComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	Super::GetCommands(OutCommands);

	const FItemData* OwnerItemData = GetOwnerItemData();
	checkf(OwnerItemData != nullptr, TEXT("Owner Item has No Data"));
	FText TextItemName = OwnerItemData->TextName;

	bool bIsEquipped = IsValid(GetOwnerCharacter()) == true;
	UEquipCommand* EquipCommand = NewObject<UEquipCommand>(this);
	EquipCommand->TargetItem = GetOwnerItem();
	EquipCommand->TextDisplay = FText::Format(
		FText::FromString(bIsEquipped == true ? TEXT("Unequip {0}") : TEXT("Equip {0}")),
		TextItemName);
	EquipCommand->bEquip = !bIsEquipped;
	EquipCommand->BodyLocation = Data->BodyLocation;
	// 인벤토리에서 공간 있는지 체크해야 함
	EquipCommand->bExecutable = true;
	OutCommands.Add(EquipCommand);
}

void UMeleeWeaponComponent::SetOwnerHoldingItemMesh()
{
	if (Data == nullptr)
		return;

	SetOwnerHoldingItemMeshAtSocket(Data->Socket);
}

void UMeleeWeaponComponent::SetOwnerAnimPreset()
{
	if (Data == nullptr)
		return;

	SetOwnerAnimPresetByHoldableType(Data->HoldableType);
}

void UMeleeWeaponComponent::OnExecuteAttack()
{
	if (Data == nullptr)
		return;

	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement();
	if (IsValid(CharacterMovement) == false)
		return;

	bIsAttacking = true;

	NetMulticast_PlayAttackMontage();

	float AttackDuration = IsValid(Data->Montage) == true ? Data->Montage->GetPlayLength() : 1.0f;
	CharacterMovement->DisableMovement();
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		EnableMovementTimerHandle,
		this,
		&ThisClass::EnableOwnerMovement,
		AttackDuration,
		false);

	OwnerCharacter->GetWorldTimerManager().SetTimer(
		CheckHitTimerHandle,
		this,
		&ThisClass::CheckHit,
		AttackDuration * CheckHitDelay,
		false);
}

void UMeleeWeaponComponent::CancelAction()
{
	UWorld* World = GetWorld();
	if (IsValid(World) == true)
	{
		World->GetTimerManager().ClearTimer(CheckHitTimerHandle);
		World->GetTimerManager().ClearTimer(EnableMovementTimerHandle);
	}

	NetMulticast_StopAttackMontage();
	EnableOwnerMovement();
}

void UMeleeWeaponComponent::OnRep_SetComponent()
{
	Super::OnRep_SetComponent();

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MeleeWeaponTable)->FindRow<FMeleeWeaponData>(ItemData->ItemId, FString(""));
}

void UMeleeWeaponComponent::CheckHit()
{
	if (Data == nullptr)
		return;

	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	HitActors.Empty();

	float AttackRadius = (Data->MaxRange - Data->MinRange) * 0.5f;
	float AttackOriginDistance = Data->MinRange + AttackRadius;
	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	FVector AttackOrigin = OwnerLocation + OwnerCharacter->GetActorForwardVector() * AttackOriginDistance;

	float MinRangeSquared = FMath::Square(Data->MinRange);
	float MaxRangeSquared = FMath::Square(Data->MaxRange);

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.bTraceComplex = true;
	FCollisionShape SphereCollision = FCollisionShape::MakeSphere(AttackRadius);

	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, AttackOrigin, AttackOrigin, FQuat::Identity, ECC_Visibility, SphereCollision, Params);
	for (const auto& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor) == false)
			continue;

		if (HitActors.Contains(HitActor) == true)
			continue;

		if (CheckIsAttackableTarget(HitActor) == false)
			continue;

		float DistSquared = FVector::DistSquared(OwnerLocation, HitActor->GetActorLocation());
		float Damage = FMath::GetMappedRangeValueClamped(
			FVector2D(MinRangeSquared, MaxRangeSquared),
			FVector2D(Data->MinDamage, Data->MaxDamage),
			DistSquared);

		FVector Dir = (HitActor->GetActorLocation() - OwnerLocation).GetSafeNormal();
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			Damage,
			Dir,
			HitResult,
			OwnerCharacter->GetController(),
			OwnerCharacter,
			nullptr);

		HitActors.Add(HitActor);
	}

	if (HitActors.IsEmpty() == false)
	{
		UDurabilityComponent* DurabilityComponent = GetOwnerItem()->GetItemComponent<UDurabilityComponent>();
		if (IsValid(DurabilityComponent) == true)
		{
			// 바꿔야 함
			float TempAmount = 1.0;
			DurabilityComponent->LoseDurability(TempAmount);
		}
	}

	// 충돌 검사 디버그
	{
		DrawDebugSphere(
			GetWorld(),
			AttackOrigin,
			AttackRadius,
			16,
			FColor::Green,
			false,
			5.0f);
	}

	/*for (const auto& HitEnemy : HitEnemies)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Hit: %s"), *HitEnemy->GetName()));
	}*/
}

bool UMeleeWeaponComponent::CheckIsAttackableTarget(AActor* TargetActor)
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return false;

	FVector Origin = OwnerCharacter->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FHitResult HitResult;
	return !GetWorld()->LineTraceSingleByObjectType(HitResult, Origin, TargetLocation, ECC_WorldStatic);
}

void UMeleeWeaponComponent::NetMulticast_PlayAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	if (IsValid(Data->Montage) == false)
		return;

	OwnerCharacter->PlayAnimMontage(Data->Montage);
}

void UMeleeWeaponComponent::NetMulticast_StopAttackMontage_Implementation()
{
	ACharacter* OwnerCharacter = GetOwnerCharacter();
	if (IsValid(OwnerCharacter) == false)
		return;

	if (IsValid(Data->Montage) == false)
		return;

	OwnerCharacter->StopAnimMontage(Data->Montage);
}
