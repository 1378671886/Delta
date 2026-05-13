// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaGameMode.h"
#include "Character/DeltaCharacter.h"
#include "Player/DeltaPlayerState.h"
#include "Player/DeltaPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaGameMode)

ADeltaGameMode::ADeltaGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = ADeltaCharacter::StaticClass();
	PlayerStateClass = ADeltaPlayerState::StaticClass();
	PlayerControllerClass = ADeltaPlayerController::StaticClass();
}
