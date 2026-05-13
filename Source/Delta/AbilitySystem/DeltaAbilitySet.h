// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "DeltaAbilitySet.generated.h"

class UDeltaAbilitySystemComponent;
class UDeltaGameplayAbility;

/**
 * FDeltaAbilitySet_GameplayAbility
 *
 * Data used by the ability set to grant a gameplay ability with its input tag.
 */
USTRUCT(BlueprintType)
struct FDeltaAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDeltaGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * FDeltaAbilitySet_GrantedHandles
 *
 * Stores handles to what has been granted by the ability set, so they can be removed later.
 */
USTRUCT(BlueprintType)
struct FDeltaAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void TakeFromAbilitySystem(UDeltaAbilitySystemComponent* ASC);

protected:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * UDeltaAbilitySet
 *
 * Non-mutable data asset used to grant gameplay abilities.
 */
UCLASS(BlueprintType, Const)
class UDeltaAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDeltaAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	void GiveToAbilitySystem(UDeltaAbilitySystemComponent* ASC, FDeltaAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FDeltaAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
