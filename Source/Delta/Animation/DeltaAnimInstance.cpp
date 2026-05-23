// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaAnimInstance.h"
#include "Character/DeltaCharacter.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "AbilitySystem/DeltaGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Weapon/Weapon.h"
#include "Components/DeltaCombatComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaAnimInstance)

UDeltaAnimInstance::UDeltaAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AimYawOffset(0.0f)
	, AimPitchOffset(0.0f)
	, RootYawOffsetMode(ERootYawOffsetMode::BlendOut)
	, PreviousActorYaw(0.0f)
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
			PreviousActorYaw = Character->GetActorRotation().Yaw;
		}
	}
}

void UDeltaAnimInstance::InitializeWithAbilitySystem(UDeltaAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	AbilitySystemComponent = ASC;
	GameplayTagPropertyMap.Initialize(this, AbilitySystemComponent);
}

void UDeltaAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	DeltaTime = DeltaSeconds;

	if (!Character || !MovementComponent)
	{
		return;
	}

	Velocity = MovementComponent->Velocity;
	Acceleration = MovementComponent->GetCurrentAcceleration();
	GroundSpeed = Velocity.Size2D();
	bHasVelocity = GroundSpeed > KINDA_SMALL_NUMBER;
	bHasAcceleration = MovementComponent->GetCurrentAcceleration().SizeSquared() > 0.0f;

	// 蹲伏——直接从MovementComponent读取
	bIsCrouching = MovementComponent->IsCrouching();
	bCrouchStateChanged = (bIsCrouching != bWasCrouching);
	bWasCrouching = bIsCrouching;

	// Tag驱动——手动查ASC（C++属性无法在编辑器PropertyMap下拉框中选取）
	bStatusChanged = false;
	if (AbilitySystemComponent)
	{
		bIsSprinting = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Sprinting);
		bIsAiming = AbilitySystemComponent->HasMatchingGameplayTag(DeltaGameplayTags::Status_Aiming);
	}

	if (bIsAiming != bWasAiming || bIsSprinting != bWasSprinting || bCrouchStateChanged)
	{
		bStatusChanged = true;
	}
	bWasAiming = bIsAiming;
	bWasSprinting = bIsSprinting;

	const FVector Accel2D = MovementComponent->GetCurrentAcceleration().GetSafeNormal2D();
	const FRotator ActorRotation = Character->GetActorRotation();
	const FVector LocalDir = ActorRotation.UnrotateVector(Accel2D);
	LocalAcceleration2D = FVector2D(LocalDir.X, LocalDir.Y);

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

	DisplacementSinceLastUpdate = GroundSpeed * DeltaSeconds;

	// 相对角色朝向的移动方向
	if (GroundSpeed > KINDA_SMALL_NUMBER)
	{
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
	UpdateAimOffset();

	AdjustedDirection = MovementDirection - RootYawOffset;

	UpdateCardinalDirection();
	UpdateJumpFallingState();
	UpdateLookAtData();
	UpdateIKHandGun();

}

void UDeltaAnimInstance::UpdateAimOffset()
{
	const AController* Controller = Character->GetController();
	if (Controller)
	{
		if (!bNeedLookAt)
		{
			const FRotator ControlRotation = Controller->GetControlRotation();
			const float ActorYaw = Character->GetActorRotation().Yaw;
			const float BodyYaw = ActorYaw + RootYawOffset;
			float InterpYawOffset;
			float InterpPitchOffset;
			InterpYawOffset = FMath::FindDeltaAngleDegrees(BodyYaw, ControlRotation.Yaw);
			InterpPitchOffset = ControlRotation.Pitch;
			if (InterpPitchOffset > 90.f)
			{
				FVector2D InRange(270.f, 360.f);
				FVector2D OutRange(-90.f, 0.f);
				InterpPitchOffset = FMath::GetMappedRangeValueClamped(InRange, OutRange, InterpPitchOffset);
			}
			AimYawOffset = FMath::FInterpTo(AimYawOffset, InterpYawOffset, DeltaTime, 5.0f);
			AimPitchOffset = FMath::FInterpTo(AimPitchOffset, InterpPitchOffset, DeltaTime, 5.0f);
		}
		else
		{
			AimYawOffset = FMath::FInterpTo(AimYawOffset, 0.0f, DeltaTime, 5.0f);
			AimPitchOffset = FMath::FInterpTo(AimPitchOffset, 0.0f, DeltaTime, 5.0f);
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

void UDeltaAnimInstance::UpdateJumpFallingState()
{
	bIsMovingOnGround = MovementComponent->IsMovingOnGround();

	if (MovementComponent->MovementMode == EMovementMode::MOVE_Falling)
	{
		if(Velocity.Z > 0.0f)
		{
			bIsJumping = true;
			bIsFalling = false;
		}
		else
		{
			bIsJumping = false;
			bIsFalling = true;
		}
	}
	else
	{
		bIsJumping = false;
		bIsFalling = false;
	}

	if (bIsJumping)
	{
		JumpApexTime = Velocity.Z * -1.0f / GetWorld()->GetGravityZ();
	}
	else
	{
		JumpApexTime = 0.0f;
	}

	if (bIsFalling)
	{
		if (MovementComponent)
		{
			FFindFloorResult FloorResult;
			MovementComponent->ComputeFloorDist(Character->GetActorLocation(), 0.0f, 10000.f, FloorResult, 1.0f);
			GroundDistance = FloorResult.FloorDist;
		}
	}
	else
	{
		GroundDistance = 0.f;
	}

}

void UDeltaAnimInstance::UpdateLookAtData()
{
	CameraComponent = Character->FindComponentByClass<UCameraComponent>();
	if (CameraComponent)
	{
		CameraLocation = CameraComponent->GetComponentLocation();
	}

	if (!bNeedTurnInPlace && FMath::Abs(RootYawOffset) > 90.0f)
	{
		bNeedLookAt = true;
	}
	else
	{
		bNeedLookAt = false;
	}
}

void UDeltaAnimInstance::UpdateIKHandGun()
{
	FVector AimSocketLocation = FVector::ZeroVector;
	FVector FPSCameraLocation = FVector::ZeroVector;
	FRotator FPSCameraRotation = FRotator::ZeroRotator;
	FVector IKHandGunLocation = FVector::ZeroVector;

	const AWeapon* Weapon = Character ? Character->GetEquippedWeapon() : nullptr;
	if (Weapon)
	{
		AimSocketLocation = Weapon->WeaponMesh->GetSocketLocation(FName("aim_socket"));
	}

	TArray<UCameraComponent*> Cameras;
	Character->GetComponents<UCameraComponent>(Cameras);
	for (const UCameraComponent* Cam : Cameras)
	{
		if (Cam && Cam->ComponentHasTag(FName("FPS")))
		{
			FPSCameraLocation = Cam->GetComponentLocation();
			FPSCameraRotation = Cam->GetComponentRotation();
			break;
		}
	}

	IKHandGunLocation = Character->GetMesh()->GetSocketLocation(FName("ik_hand_gun"));
	const FRotator IKHandGunRotation = Character->GetMesh()->GetSocketRotation(FName("ik_hand_gun"));

	const float Alpha = Character->CombatComponent ? Character->CombatComponent->AimAlpha : 0.0f;
	const FVector DeltaLocation = AimSocketLocation - FPSCameraLocation;
	const FVector ADSTargetLocation = IKHandGunLocation - DeltaLocation;
	const FRotator ADSTargetRotation = FRotator(-FPSCameraRotation.Pitch, FPSCameraRotation.Yaw + 180.0f, FPSCameraRotation.Roll);

	ADSIKHandGunLocation = FMath::Lerp(IKHandGunLocation, ADSTargetLocation, Alpha);
	ADSIKHandGunRotation = FMath::Lerp(IKHandGunRotation, ADSTargetRotation, Alpha);
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
