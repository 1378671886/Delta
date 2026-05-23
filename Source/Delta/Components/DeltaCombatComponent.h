// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeltaCombatComponent.generated.h"

class UDeltaAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELTA_API UDeltaCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeltaCombatComponent();

	// Time in seconds to fully transition into/out of ADS
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|ADS")
	float ADSTime = 0.15f;

	// Current ADS blend alpha: 0 = hip-fire, 1 = fully aimed
	UPROPERTY(BlueprintReadOnly, Category = "Combat|ADS")
	float AimAlpha = 0.0f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UDeltaAbilitySystemComponent> AbilitySystemComponent;
};
