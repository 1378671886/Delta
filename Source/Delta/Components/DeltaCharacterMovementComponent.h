// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "DeltaCharacterMovementComponent.generated.h"

UCLASS(MinimalAPI, Config = Game)
class UDeltaCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UDeltaCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

	//~UCharacterMovementComponent interface
	virtual bool CanAttemptJump() const override;
	//~End of UCharacterMovementComponent interface

	// Sprint speed in cm/s. Default 1200 (12 m/s, roughly 2x walk speed).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delta|Sprint")
	float MaxSprintSpeed = 1200.0f;

	// ADS walk speed in cm/s. Default 300 (3 m/s).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delta|Aim")
	float MaxAimSpeed = 300.0f;

protected:
	virtual void InitializeComponent() override;
};
