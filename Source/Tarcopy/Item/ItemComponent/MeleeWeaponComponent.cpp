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

const float UMeleeWeaponComponent::CheckHitDelay = 0.6f;

void UMeleeWeaponComponent::SetOwnerItem(UItemInstance* InOwnerItem)
{
	Super::SetOwnerItem(InOwnerItem);

	const FItemData* ItemData = GetOwnerItemData();
	if (ItemData == nullptr)
		return;

	UDataTableSubsystem* DataTableSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDataTableSubsystem>();
	if (IsValid(DataTableSubsystem) == false)
		return;

	Data = DataTableSubsystem->GetTable(EDataTableType::MeleeWeaponTable)->FindRow<FMeleeWeaponData>(ItemData->ItemId, FString(""));
	if (Data == nullptr)
		return;
}

void UMeleeWeaponComponent::GetCommands(TArray<TObjectPtr<class UItemCommandBase>>& OutCommands)
{
	Super::GetCommands(OutCommands);
}

void UMeleeWeaponComponent::ExecuteAttack(ACharacter* OwnerCharacter)
{
	if (IsValid(OwnerCharacter) == false)
		return;

	UCharacterMovementComponent* CharacterMovement = OwnerCharacter->GetCharacterMovement();
	if (IsValid(CharacterMovement) == false)
		return;

	bIsAttacking = true;
	CachedOwner = OwnerCharacter;

	if (IsValid(Data->Montage) == true)
	{
		OwnerCharacter->PlayAnimMontage(Data->Montage);
	}

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

	EnableOwnerMovement();
}

void UMeleeWeaponComponent::CheckHit()
{
	if (Data == nullptr)
		return;

	if (CachedOwner.IsValid() == false)
		return;

	ACharacter* OwnerCharacter = CachedOwner.Get();

	UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Check Hit"));

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
	if (CachedOwner.IsValid() == false)
		return false;

	FVector Origin = CachedOwner->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FHitResult HitResult;
	return !GetWorld()->LineTraceSingleByObjectType(HitResult, Origin, TargetLocation, ECC_WorldStatic);
}

void UMeleeWeaponComponent::EnableOwnerMovement()
{
	if (bIsAttacking == false)
		return;

	if (CachedOwner.IsValid() == false)
		return;

	UCharacterMovementComponent* CharacterMovement = CachedOwner->GetCharacterMovement();
	if (IsValid(CharacterMovement) == false)
		return;

	bIsAttacking = false;
	CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking);
}
