// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayAbility_Sprint.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaGameplayAbility_Sprint)

UDeltaGameplayAbility_Sprint::UDeltaGameplayAbility_Sprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ActivationPolicy = EDeltaAbilityActivationPolicy::WhileInputActive;
}

void UDeltaGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Sprinting, 1);
	}

	if (ActivationPolicy == EDeltaAbilityActivationPolicy::OnInputTriggered)
	{
		// Prevent InputPressed (which fires every frame while held) from immediately toggling off
		bInputPressHandled = true;
	}
}

void UDeltaGameplayAbility_Sprint::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActivationPolicy == EDeltaAbilityActivationPolicy::OnInputTriggered)
	{
		if (!bInputPressHandled)
		{
			bInputPressHandled = true;

			if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
			{
				const bool bIsSprinting = ASC->HasMatchingGameplayTag(DeltaGameplayTags::Status_Sprinting);
				ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Sprinting, bIsSprinting ? 0 : 1);
			}
		}
	}
}

void UDeltaGameplayAbility_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bInputPressHandled = false;

	if (ActivationPolicy == EDeltaAbilityActivationPolicy::WhileInputActive)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UDeltaGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UDeltaAbilitySystemComponent* ASC = GetDeltaAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(DeltaGameplayTags::Status_Sprinting, 0);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
