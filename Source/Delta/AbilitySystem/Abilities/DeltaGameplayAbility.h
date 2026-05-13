// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "DeltaGameplayAbility.generated.h"

class UDeltaAbilitySystemComponent;
class ADeltaCharacter;
class ADeltaPlayerController;
class UDeltaHeroComponent;

/**
 * EDeltaAbilityActivationPolicy
 *
 * Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EDeltaAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * EDeltaAbilityActivationGroup
 *
 * Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EDeltaAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,
};

/**
 * UDeltaGameplayAbility
 *
 * Base gameplay ability class for the Delta project.
 */
UCLASS(MinimalAPI, Abstract, HideCategories = Input)
class UDeltaGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UDeltaAbilitySystemComponent;

public:
	DELTA_API UDeltaGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	DELTA_API UDeltaAbilitySystemComponent* GetDeltaAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	DELTA_API ADeltaPlayerController* GetDeltaPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	DELTA_API AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	DELTA_API ADeltaCharacter* GetDeltaCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Delta|Ability")
	DELTA_API UDeltaHeroComponent* GetHeroComponentFromActorInfo() const;

	EDeltaAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EDeltaAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	DELTA_API void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:
	//~UGameplayAbility interface
	DELTA_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	DELTA_API virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	DELTA_API virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	DELTA_API virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
	DELTA_API void K2_OnAbilityAdded();

	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
	DELTA_API void K2_OnAbilityRemoved();

	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	DELTA_API void K2_OnPawnAvatarSet();

protected:
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Delta|Ability Activation")
	EDeltaAbilityActivationPolicy ActivationPolicy = EDeltaAbilityActivationPolicy::OnInputTriggered;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Delta|Ability Activation")
	EDeltaAbilityActivationGroup ActivationGroup = EDeltaAbilityActivationGroup::Independent;
};
