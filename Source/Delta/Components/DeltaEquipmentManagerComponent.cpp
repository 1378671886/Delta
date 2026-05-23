// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DeltaEquipmentManagerComponent.h"
#include "Components/DeltaHeroComponent.h"
#include "Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

UDeltaEquipmentManagerComponent::UDeltaEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDeltaEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDeltaEquipmentManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unequip all weapons before destruction
	TArray<TObjectPtr<AWeapon>> CopyOfWeapons(EquippedWeapons);
	for (const TObjectPtr<AWeapon>& W : CopyOfWeapons)
	{
		if (W.Get() != nullptr)
		{
			UnequipWeapon(W.Get());
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UDeltaEquipmentManagerComponent::EquipWeapon(AWeapon* Weapon)
{
	if (!Weapon || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (EquippedWeapons.Contains(Weapon))
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* CharacterMesh = Character ? Character->GetMesh() : nullptr;
	if (!CharacterMesh)
	{
		return;
	}

	Weapon->SetOwner(Character);
	Weapon->WeaponMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("firearm_socket"));
	Weapon->CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetActorEnableCollision(false);

	// Grant weapon abilities
	if (Weapon->EquipmentAbilitySet)
	{
		if (UDeltaAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			Weapon->EquipmentAbilitySet->GiveToAbilitySystem(ASC, &Weapon->GrantedHandles, Weapon);
		}
	}

	// Add weapon input mapping to override base IMC
	if (Weapon->EquipmentInputMapping)
	{
		if (UDeltaHeroComponent* HC = UDeltaHeroComponent::FindHeroComponent(GetOwner()))
		{
			HC->AddEquipmentInputMapping(Weapon->EquipmentInputMapping);
		}
	}

	// Link weapon animation layer
	if (Weapon->EquipmentAnimLayer)
	{
		if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
		{
			AnimInstance->LinkAnimClassLayers(Weapon->EquipmentAnimLayer);
		}
	}

	EquippedWeapons.Add(Weapon);
}

void UDeltaEquipmentManagerComponent::UnequipWeapon(AWeapon* Weapon)
{
	if (!Weapon || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	if (!EquippedWeapons.Contains(Weapon))
	{
		return;
	}

	// Unlink weapon animation layer
	if (Weapon->EquipmentAnimLayer)
	{
		ACharacter* Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			if (USkeletalMeshComponent* CharacterMesh = Character->GetMesh())
			{
				if (UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance())
				{
					AnimInstance->UnlinkAnimClassLayers(Weapon->EquipmentAnimLayer);
				}
			}
		}
	}

	// Remove weapon input mapping
	if (Weapon->EquipmentInputMapping)
	{
		if (UDeltaHeroComponent* HC = UDeltaHeroComponent::FindHeroComponent(GetOwner()))
		{
			HC->RemoveEquipmentInputMapping(Weapon->EquipmentInputMapping);
		}
	}

	// Remove granted abilities
	if (UDeltaAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		Weapon->GrantedHandles.TakeFromAbilitySystem(ASC);
	}

	Weapon->WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Weapon->SetOwner(nullptr);
	Weapon->CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Weapon->SetActorEnableCollision(true);

	EquippedWeapons.Remove(Weapon);
}

bool UDeltaEquipmentManagerComponent::TryPickUpWeapon()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->HasAuthority())
	{
		return false;
	}

	for (TActorIterator<AWeapon> It(GetWorld()); It; ++It)
	{
		AWeapon* Weapon = *It;
		if (Weapon && Weapon->CanBePickedUpBy(Pawn))
		{
			Weapon->PickUp(Pawn);
			return true;
		}
	}

	return false;
}

bool UDeltaEquipmentManagerComponent::HasNearbyWeapon() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return false;
	}

	for (TActorIterator<AWeapon> It(GetWorld()); It; ++It)
	{
		AWeapon* Weapon = *It;
		if (Weapon && Weapon->CanBePickedUpBy(Pawn))
		{
			return true;
		}
	}

	return false;
}

TArray<AWeapon*> UDeltaEquipmentManagerComponent::GetEquippedWeapons() const
{
	TArray<AWeapon*> Result;
	for (const TObjectPtr<AWeapon>& W : EquippedWeapons)
	{
		if (W.Get() != nullptr)
		{
			Result.Add(W.Get());
		}
	}
	return Result;
}



UDeltaAbilitySystemComponent* UDeltaEquipmentManagerComponent::GetAbilitySystemComponent() const
{
	return Cast<UDeltaAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}
