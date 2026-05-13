// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/DeltaGameplayAbility.h"
#include "DeltaGameplayAbility_Jump.generated.h"

/**
 * UDeltaGameplayAbility_Jump
 *
 * Gameplay ability used for character jumping. Follows Lyra's pattern:
 * - CharacterJumpStart on activation, CharacterJumpStop on end / cancel.
 * - BlueprintImplementableEvent on the Jump ability itself handles the animation.
 */
UCLASS(Abstract)
class UDeltaGameplayAbility_Jump : public UDeltaGameplayAbility
{
	GENERATED_BODY()

public:
	UDeltaGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	void CharacterJumpStop();
};
