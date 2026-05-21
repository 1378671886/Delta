// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayAbility_Interact.h"
#include "Character/DeltaCharacter.h"
#include "Components/DeltaEquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaGameplayAbility_Interact)

UDeltaGameplayAbility_Interact::UDeltaGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UDeltaGameplayAbility_Interact::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ADeltaCharacter* DeltaCharacter = GetDeltaCharacterFromActorInfo();
	if (!DeltaCharacter || !DeltaCharacter->EquipmentManagerComponent)
	{
		return false;
	}

	return DeltaCharacter->EquipmentManagerComponent->HasNearbyWeapon();
}

void UDeltaGameplayAbility_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ADeltaCharacter* DeltaCharacter = GetDeltaCharacterFromActorInfo())
	{
		if (UDeltaEquipmentManagerComponent* EquipmentMgr = DeltaCharacter->EquipmentManagerComponent)
		{
			EquipmentMgr->TryPickUpWeapon();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
