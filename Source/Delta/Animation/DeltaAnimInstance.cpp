// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaAnimInstance.h"
#include "Character/DeltaCharacter.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaAnimInstance)

UDeltaAnimInstance::UDeltaAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, PreviousActorYaw(0.0f)
	, RootYawOffsetMode(ERootYawOffsetMode::BlendOut)
	, AimYawOffset(0.0f)
	, AimPitchOffset(0.0f)
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
			PreviousActorYaw = Character->GetActorRotation().Yaw;
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

	// 速度
	Velocity = MovementComponent->Velocity;
	Acceleration = MovementComponent->GetCurrentAcceleration();
	GroundSpeed = Velocity.Size2D();
	bIsMovingOnGround = MovementComponent->IsMovingOnGround();
	bHasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;

	// 加速度：角色正在提供移动输入时为 true（而非滑行/减速）
	bHasAcceleration = MovementComponent->GetCurrentAcceleration().SizeSquared() > 0.0f;

	// 本地加速度方向
	{
		const FVector Accel2D = MovementComponent->GetCurrentAcceleration().GetSafeNormal2D();
		const FRotator ActorRotation = Character->GetActorRotation();
		const FVector LocalDir = ActorRotation.UnrotateVector(Accel2D);
		LocalAcceleration2D = FVector2D(LocalDir.X, LocalDir.Y);

		// 加速度基准的转身方向：与输入方向相反
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

	// 相对角色朝向的移动方向
	if (GroundSpeed > KINDA_SMALL_NUMBER)
	{
		const FRotator ActorRotation = Character->GetActorRotation();
		const FVector VelocityDirection = Velocity.GetSafeNormal2D();
		const FVector LocalDirection = ActorRotation.UnrotateVector(VelocityDirection);

		// 角度：前=0，右=90，左=-90
		MovementDirection = FMath::RadiansToDegrees(FMath::Atan2(LocalDirection.Y, LocalDirection.X));
		LocalVelocity2D = FVector2D(LocalDirection.X, LocalDirection.Y);
	}
	else
	{
		MovementDirection = 0.0f;
		LocalVelocity2D = FVector2D::ZeroVector;
	}

	UpdateRootYawOffset(DeltaSeconds);

	// 瞄准偏移：Controller朝向与角色朝向的差值
	UpdateAimOffset();

	AdjustedDirection = MovementDirection - RootYawOffset;

	UpdateCardinalDirection();

	// GameplayTag驱动的状态
	if (AbilitySystemComponent)
	{
		bIsCrouching = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Crouching);
		bIsSprinting = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Sprinting);
	}
}

void UDeltaAnimInstance::UpdateAimOffset()
{
	const AController* Controller = Character->GetController();
	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const float ActorYaw = Character->GetActorRotation().Yaw;
		const float BodyYaw = ActorYaw + RootYawOffset;
		AimYawOffset = FMath::FindDeltaAngleDegrees(BodyYaw, ControlRotation.Yaw);
		AimPitchOffset = ControlRotation.Pitch;
		if (AimPitchOffset > 90.f)
		{
			FVector2D InRange(270.f, 360.f);
			FVector2D OutRange(-90.f, 0.f);
			AimPitchOffset = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimPitchOffset);
		}
	}
	else
	{
		AimYawOffset = 0.0f;
		AimPitchOffset = 0.0f;
	}
}

void UDeltaAnimInstance::UpdateCardinalDirection()
{
	if (AdjustedDirection >= -45.0f && AdjustedDirection <= 45.0f)
	{
		CardinalDirection = ECardinalDirection::Front;
	}
	else if (AdjustedDirection > 45.0f && AdjustedDirection <= 135.0f)
	{
		CardinalDirection = ECardinalDirection::Right;
	}
	else if (AdjustedDirection < -45.0f && AdjustedDirection >= -135.0f)
	{
		CardinalDirection = ECardinalDirection::Left;
	}
	else
	{
		CardinalDirection = ECardinalDirection::Back;
	}
}

void UDeltaAnimInstance::UpdateRootYawOffset(float DeltaSeconds)
{
	const float CurrentActorYaw = Character->GetActorRotation().Yaw;
	RootYawDelta = FMath::FindDeltaAngleDegrees(PreviousActorYaw, CurrentActorYaw);
	PreviousActorYaw = CurrentActorYaw;

	switch (RootYawOffsetMode)
	{
	case ERootYawOffsetMode::Accumulate:
		RootYawOffset = FMath::UnwindDegrees(RootYawOffset - RootYawDelta);
		//RootYawOffset = FMath::Clamp(RootYawOffset, -120.0f, 120.0f);
		break;
	case ERootYawOffsetMode::HoldOn:
		break;
	case ERootYawOffsetMode::BlendOut:
		RootYawOffset = UKismetMathLibrary::FloatSpringInterp(RootYawOffset, 0.0f, RootYawSpringState, 80.0f, 1.0f, DeltaSeconds, 1.0f, 0.5f);
		//RootYawOffset = FMath::Clamp(RootYawOffset, -120.0f, 120.0f);
		break;
	default:
		break;
	}

	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}
