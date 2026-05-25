// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeltaCombatComponent.generated.h"

class UDeltaAbilitySystemComponent;
class UCameraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELTA_API UDeltaCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeltaCombatComponent();

	// FOV when not aiming (hip-fire)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|ADS")
	float DefaultFOV = 90.0f;

	// FOV when fully aimed down sights
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|ADS")
	float ADSFOV = 60.0f;

	// Time to transition weapon from hip to ADS position
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat|ADS")
	float ADSTime = 0.15f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TObjectPtr<UDeltaAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraComponent;
};
