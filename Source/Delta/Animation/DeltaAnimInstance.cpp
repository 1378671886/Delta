// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaAnimInstance.h"
#include "Character/DeltaCharacter.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaAnimInstance)

UDeltaAnimInstance::UDeltaAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDeltaAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		Character = Cast<ADeltaCharacter>(OwningActor);
		if (Character)
		{
			MovementComponent = Character->GetCharacterMovement();
			AbilitySystemComponent = Character->GetDeltaAbilitySystemComponent();
		}
	}
}

void UDeltaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	DeltaTime = DeltaSeconds;

	if (!Character || !MovementComponent)
	{
		return;
	}

	// Velocity
	Velocity = MovementComponent->Velocity;
	Acceleration = MovementComponent->GetCurrentAcceleration();
	GroundSpeed = Velocity.Size2D();
	bIsMovingOnGround = MovementComponent->IsMovingOnGround();
	bHasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;

	// Acceleration: true when the character is providing movement input
	// (not just coasting/decelerating)
	bHasAcceleration = MovementComponent->GetCurrentAcceleration().SizeSquared() > 0.0f;

	// Local acceleration direction
	{
		const FVector Accel2D = MovementComponent->GetCurrentAcceleration().GetSafeNormal2D();
		const FRotator ActorRotation = Character->GetActorRotation();
		const FVector LocalDir = ActorRotation.UnrotateVector(Accel2D);
		LocalAcceleration2D = FVector2D(LocalDir.X, LocalDir.Y);

		// Pivot direction: opposite of acceleration input
		{
			const float AccelAngle = FMath::RadiansToDegrees(FMath::Atan2(LocalAcceleration2D.Y, LocalAcceleration2D.X));
			if (AccelAngle >= -45.0f && AccelAngle <= 45.0f)
			{
				PivotDirection = ECardinalDirection::Back;
			}
			else if (AccelAngle > 45.0f && AccelAngle <= 135.0f)
			{
				PivotDirection = ECardinalDirection::Left;
			}
			else if (AccelAngle < -45.0f && AccelAngle >= -135.0f)
			{
				PivotDirection = ECardinalDirection::Right;
			}
			else
			{
				PivotDirection = ECardinalDirection::Front;
			}
		}
	}

	DisplacementSinceLastUpdate = GroundSpeed * DeltaSeconds;

	// Movement direction relative to character facing
	if (GroundSpeed > KINDA_SMALL_NUMBER)
	{
		const FRotator ActorRotation = Character->GetActorRotation();
		const FVector VelocityDirection = Velocity.GetSafeNormal2D();
		const FVector LocalDirection = ActorRotation.UnrotateVector(VelocityDirection);

		// Angle: forward=0, right=90, left=-90
		MovementDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalDirection.Y, LocalDirection.X));
		LocalVelocity2D = FVector2D(LocalDirection.X, LocalDirection.Y);
	}
	else
	{
		MovementDirection = 0.0f;
		LocalVelocity2D = FVector2D::ZeroVector;
	}

	// Cardinal direction from movement angle
	if (MovementDirection >= -45.0f && MovementDirection <= 45.0f)
	{
		CardinalDirection = ECardinalDirection::Front;
	}
	else if (MovementDirection > 45.0f && MovementDirection <= 135.0f)
	{
		CardinalDirection = ECardinalDirection::Right;
	}
	else if (MovementDirection < -45.0f && MovementDirection >= -135.0f)
	{
		CardinalDirection = ECardinalDirection::Left;
	}
	else
	{
		CardinalDirection = ECardinalDirection::Back;
	}

	// Gameplay-tag-driven states
	if (AbilitySystemComponent)
	{
		bIsCrouching = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Crouching);
		bIsSprinting = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Sprinting);
	}
}
