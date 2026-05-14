// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "DeltaAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	Front	UMETA(DisplayName = "前"),
	Back	UMETA(DisplayName = "后"),
	Left	UMETA(DisplayName = "左"),
	Right	UMETA(DisplayName = "右")
};

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
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	TObjectPtr<ADeltaCharacter> Character;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	TObjectPtr<UDeltaAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	FVector Acceleration;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsMovingOnGround;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bHasVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bHasAcceleration;

	//距离匹配 调整显式时间
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float DisplacementSinceLastUpdate;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float MovementDirection;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	FVector2D LocalVelocity2D;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	FVector2D LocalAcceleration2D;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	ECardinalDirection CardinalDirection;

	// Pivot direction from acceleration: opposite of input direction.
	// Accel backward → Front, Accel left → Right, etc.
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	ECardinalDirection PivotDirection;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsSprinting;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float DeltaTime;
};
