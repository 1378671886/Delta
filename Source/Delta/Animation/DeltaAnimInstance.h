// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "DeltaAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	Front	UMETA(DisplayName = "前"),
	Back	UMETA(DisplayName = "后"),
	Left	UMETA(DisplayName = "左"),
	Right	UMETA(DisplayName = "右")
};

UENUM(BlueprintType)
enum class ERootYawOffsetMode : uint8
{
	Accumulate	UMETA(DisplayName = "累积"),
	HoldOn		UMETA(DisplayName = "保持"),
	BlendOut	UMETA(DisplayName = "混出")
};

class ADeltaCharacter;
class UCharacterMovementComponent;
class UDeltaAbilitySystemComponent;

/**
 * UDeltaAnimInstance
 *
 * Delta角色的动画实例。初始化时缓存角色引用，每帧更新动画相关数据——
 * 速度、加速度、地面速度、移动方向和GameplayTag驱动的状态。
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

	// 距离匹配 调整显式时间
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

	// 加速度基准的转身方向：与输入方向相反。
	// 加速向后 → 前，加速向左 → 右，以此类推。
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	ECardinalDirection PivotDirection;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	bool bIsSprinting;

	// 每帧Yaw旋转增量（度），正值右转，负值左转
	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float RootYawDelta;

	// 累计Yaw偏移，归一化到[-180, 180]，用于AnimBP旋转根骨骼实现原地转身
	UPROPERTY(BlueprintReadWrite, Category = "Delta|Anim")
	float RootYawOffset;

	// RootYawOffset的累积/保持/混出模式
	UPROPERTY(BlueprintReadWrite, Category = "Delta|Anim")
	ERootYawOffsetMode RootYawOffsetMode;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float DeltaTime;

	UPROPERTY(BlueprintReadOnly, Category = "Delta|Anim")
	float AdjustedDirection;

	void UpdateRootYawOffset(float DeltaSeconds);

	void UpdateCardinalDirection();

	UPROPERTY(BlueprintReadWrite, Category = "Delta|Anim")
	float TurnInPlaceRotationTime;

private:
	float PreviousActorYaw;
	FFloatSpringState RootYawSpringState;
};
