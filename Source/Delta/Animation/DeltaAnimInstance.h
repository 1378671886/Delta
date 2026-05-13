// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "DeltaAnimInstance.generated.h"

class ADeltaCharacter;
class UCharacterMovementComponent;
class UDeltaAbilitySystemComponent;

/**
 * UDeltaAnimInstance
 *
 * Anim instance for Delta characters. Caches character references on init and
 * updates animation-relevant data each frame — velocity, acceleration, ground speed,
 * movement direction, and gameplay-tag-driven states.
 */
UCLASS(BlueprintType, Blueprintable)
class DELTA_API UDeltaAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UDeltaAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	//~End of UAnimInstance interface

protected:
	// Cached — set once in NativeInitializeAnimation
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	TObjectPtr<ADeltaCharacter> Character;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	TObjectPtr<UDeltaAbilitySystemComponent> AbilitySystemComponent;

	// Updated every frame
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsMovingOnGround;

	// True when the character is providing input acceleration (has input intent to move)
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bHasAcceleration;

	// Movement direction angle relative to character facing, in degrees [-180, 180].
	// 0 = forward, 90 = right, -90 = left, +/-180 = backward.
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float MovementDirection;

	// Movement direction in local space (relative to actor rotation), normalized.
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	FVector2D LocalVelocity2D;

	// Cached gameplay tag states for anim graph use
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsSprinting;

	// Native tick
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float DeltaTime;
};
