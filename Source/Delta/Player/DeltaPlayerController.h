// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "DeltaPlayerController.generated.h"

UCLASS(MinimalAPI, Config = Game)
class ADeltaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADeltaPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Delta|PlayerController")
	void SetIsAutoRunning(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Delta|PlayerController")
	bool GetIsAutoRunning() const;

protected:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	void ProcessAutoRun(float DeltaTime);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

private:
	bool bIsAutoRunning = false;
};
