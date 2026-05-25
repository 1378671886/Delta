// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/DeltaGameplayAbility.h"
#include "DeltaGameplayAbility_Aim.generated.h"

class UCameraComponent;

UCLASS(Abstract)
class DELTA_API UDeltaGameplayAbility_Aim : public UDeltaGameplayAbility
{
	GENERATED_BODY()

public:
	UDeltaGameplayAbility_Aim(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// Component tag to identify the first-person camera on the character.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Camera")
	FName FPSCameraTag = TEXT("FPS");

	void EnterFPS(ADeltaCharacter* Char);
	void ExitFPS();

	UPROPERTY()
	TObjectPtr<UCameraComponent> FPSCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> TPSCamera;

	bool bFPSWasActive;
	bool bTPSWasActive;
};
