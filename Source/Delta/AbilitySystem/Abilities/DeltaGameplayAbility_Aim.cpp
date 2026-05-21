// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayAbility_Aim.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaGameplayAbility_Aim)

UDeltaGameplayAbility_Aim::UDeltaGameplayAbility_Aim(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationPolicy = EDeltaAbilityActivationPolicy::WhileInputActive;
}

void UDeltaGameplayAbility_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Aiming, 1);
	}
}

void UDeltaGameplayAbility_Aim::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActivationPolicy == EDeltaAbilityActivationPolicy::WhileInputActive)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UDeltaGameplayAbility_Aim::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Aiming, 0);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
