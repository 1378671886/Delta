// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/DeltaGameplayAbility.h"
#include "DeltaGameplayAbility_Sprint.generated.h"

/**
 * UDeltaGameplayAbility_Sprint
 *
 * Sprint ability that supports both hold and toggle via ActivationPolicy:
 *   WhileInputActive  - hold to sprint, release to walk
 *   OnInputTriggered  - press to toggle sprint on/off
 *
 * Sets Status.Sprinting tag; CharacterMovementComponent reads it to boost MaxWalkSpeed.
 *
 * Configure AbilityTags (e.g. "Ability.Type.Sprint") in blueprints
 * to manage blocking/cancellation relationships with other abilities.
 */
UCLASS(Abstract)
class UDeltaGameplayAbility_Sprint : public UDeltaGameplayAbility
{
	GENERATED_BODY()

public:
	UDeltaGameplayAbility_Sprint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	// Debounce flag: true after handling an InputPressed in OnInputTriggered (toggle) mode.
	// Reset on InputReleased so the next press toggles again.
	bool bInputPressHandled = false;
};
