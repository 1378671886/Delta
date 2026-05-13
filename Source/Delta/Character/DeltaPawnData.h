// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "DeltaPawnData.generated.h"

class APawn;
class UDeltaAbilitySet;
class UDeltaInputConfig;

/**
 * UDeltaPawnData
 *
 * Non-mutable data asset that defines a pawn's configuration:
 * abilities, input, and pawn class.
 */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "Delta Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class UDeltaPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	DELTA_API UDeltaPawnData(const FObjectInitializer& ObjectInitializer);

	// Class to instantiate for this pawn (should derive from ADeltaCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Delta|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Delta|Abilities")
	TArray<TObjectPtr<UDeltaAbilitySet>> AbilitySets;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Delta|Input")
	TObjectPtr<UDeltaInputConfig> InputConfig;
};
