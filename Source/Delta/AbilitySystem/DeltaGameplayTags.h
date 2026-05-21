// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace DeltaGameplayTags
{
	// Input Tags — bound to EnhancedInput Actions
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_HoldCrouch);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Jump);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interact);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Aim);

	// Status Tags
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Aiming);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Sprinting);

	// Ability Tags — used in AbilityTags on GA blueprints for blocking/cancellation
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Jump);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Sprint);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interact);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Aim);

	// Movement control — set by GAS abilities to freeze movement
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

	// Movement Mode Tags — synced to ASC when movement mode changes
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
	DELTA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);

	// MovementMode enum → GameplayTag mapping tables
	DELTA_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	DELTA_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
};
