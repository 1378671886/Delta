// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/DeltaGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaCharacterMovementComponent)

UDeltaCharacterMovementComponent::UDeltaCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDeltaCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

FRotator UDeltaCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(DeltaGameplayTags::TAG_Gameplay_MovementStopped))
		{
			return FRotator::ZeroRotator;
		}
	}

	return Super::GetDeltaRotation(DeltaTime);
}

float UDeltaCharacterMovementComponent::GetMaxSpeed() const
{
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(DeltaGameplayTags::TAG_Gameplay_MovementStopped))
		{
			return 0.0f;
		}

		if (ASC->HasMatchingGameplayTag(DeltaGameplayTags::Status_Aiming))
		{
			return MaxAimSpeed;
		}

		if (ASC->HasMatchingGameplayTag(DeltaGameplayTags::Status_Sprinting))
		{
			return MaxSprintSpeed;
		}
	}

	return Super::GetMaxSpeed();
}

bool UDeltaCharacterMovementComponent::CanAttemptJump() const
{
	// Same as UCharacterMovementComponent but without the crouch check — allows jumping while crouched
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());
}
