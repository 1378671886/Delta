// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameplayTags.h"

namespace DeltaGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Crouch, "InputTag.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_HoldCrouch, "InputTag.HoldCrouch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_AutoRun, "InputTag.AutoRun");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Jump, "InputTag.Jump");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Sprint, "InputTag.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Aim, "InputTag.Aim");

	UE_DEFINE_GAMEPLAY_TAG(Status_Crouching, "Status.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Status_Aiming, "Status.Aiming");
	UE_DEFINE_GAMEPLAY_TAG(Status_AutoRunning, "Status.AutoRunning");
	UE_DEFINE_GAMEPLAY_TAG(Status_Sprinting, "Status.Sprinting");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Sprint, "Ability.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact, "Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Aim, "Ability.Aim");

	UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Walking, "Movement.Mode.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_NavWalking, "Movement.Mode.NavWalking");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Falling, "Movement.Mode.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Swimming, "Movement.Mode.Swimming");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Flying, "Movement.Mode.Flying");
	UE_DEFINE_GAMEPLAY_TAG(Movement_Mode_Custom, "Movement.Mode.Custom");

	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_Walking, Movement_Mode_Walking },
		{ MOVE_NavWalking, Movement_Mode_NavWalking },
		{ MOVE_Falling, Movement_Mode_Falling },
		{ MOVE_Swimming, Movement_Mode_Swimming },
		{ MOVE_Flying, Movement_Mode_Flying },
		{ MOVE_Custom, Movement_Mode_Custom }
	};

	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
	{
		// Fill in with custom movement modes as needed
	};
};
