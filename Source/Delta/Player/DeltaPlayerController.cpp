// Copyright Epic Games, Inc. All Rights Reserved.

#include "DeltaPlayerController.h"
#include "AbilitySystem/DeltaAbilitySystemComponent.h"
#include "Player/DeltaPlayerState.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DeltaPlayerController)

ADeltaPlayerController::ADeltaPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ADeltaPlayerController::SetIsAutoRunning(bool bEnabled)
{
	bIsAutoRunning = bEnabled;
}

bool ADeltaPlayerController::GetIsAutoRunning() const
{
	return bIsAutoRunning;
}

void ADeltaPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	ProcessAutoRun(DeltaTime);
}

void ADeltaPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	ProcessAbilityInput(DeltaTime, bGamePaused);
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ADeltaPlayerController::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if (const ADeltaPlayerState* DeltaPS = GetPlayerState<ADeltaPlayerState>())
	{
		if (UDeltaAbilitySystemComponent* ASC = DeltaPS->GetDeltaAbilitySystemComponent())
		{
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
}

void ADeltaPlayerController::ProcessAutoRun(float DeltaTime)
{
	if (!bIsAutoRunning)
	{
		return;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return;
	}

	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = YawRotation.RotateVector(FVector::ForwardVector);
	MyPawn->AddMovementInput(ForwardDirection, 1.0f);
}
