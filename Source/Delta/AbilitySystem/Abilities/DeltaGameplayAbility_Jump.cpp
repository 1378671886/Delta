// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayAbility_Jump.h"
#include "Character/DeltaCharacter.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaGameplayAbility_Jump)

UDeltaGameplayAbility_Jump::UDeltaGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UDeltaGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ADeltaCharacter* DeltaCharacter = Cast<ADeltaCharacter>(ActorInfo->AvatarActor.Get());
	if (!DeltaCharacter || !DeltaCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UDeltaGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDeltaGameplayAbility_Jump::CharacterJumpStart()
{
	if (ADeltaCharacter* DeltaCharacter = GetDeltaCharacterFromActorInfo())
	{
		if (DeltaCharacter->IsLocallyControlled() && !DeltaCharacter->bPressedJump)
		{
			DeltaCharacter->UnCrouch();
			DeltaCharacter->Jump();
		}
	}
}

void UDeltaGameplayAbility_Jump::CharacterJumpStop()
{
	if (ADeltaCharacter* DeltaCharacter = GetDeltaCharacterFromActorInfo())
	{
		if (DeltaCharacter->IsLocallyControlled() && DeltaCharacter->bPressedJump)
		{
			DeltaCharacter->StopJumping();
		}
	}
}
