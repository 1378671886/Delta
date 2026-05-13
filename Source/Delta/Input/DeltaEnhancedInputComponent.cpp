// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaEnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaEnhancedInputComponent)

UDeltaEnhancedInputComponent::UDeltaEnhancedInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDeltaEnhancedInputComponent::AddInputMappings(const UDeltaInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	// Add any player-mappable key mappings from the config
	// Currently a placeholder — extend if using player-mappable keys
}
