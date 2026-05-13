// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "DeltaGameMode.generated.h"

UCLASS(MinimalAPI, Config = Game)
class ADeltaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADeltaGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
