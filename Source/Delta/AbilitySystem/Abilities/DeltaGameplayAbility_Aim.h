// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/DeltaGameplayAbility.h"
#include "DeltaGameplayAbility_Aim.generated.h"

class UCameraComponent;
class USkeletalMeshComponent;

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

	// Weapon location and rotation offset from camera when aiming.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Weapon")
	FVector WeaponCameraOffset = FVector(100.0f, 0.0f, -20.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Weapon")
	FRotator WeaponCameraRotation = FRotator(0.0, -90.0, 0.0);

	// Socket on character mesh to reattach weapon to when un-aiming.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim|Weapon")
	FName WeaponAttachSocket = TEXT("firearm_socket");

private:
	void EnterFPS(ADeltaCharacter* Char);
	void ExitFPS();
	void AttachWeaponToCamera(ADeltaCharacter* Char);
	void AttachWeaponToMesh(ADeltaCharacter* Char);
	void TickWeaponBlend();

	UPROPERTY()
	TObjectPtr<UCameraComponent> FPSCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> TPSCamera;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	FTimerHandle WeaponBlendTimer;
	float WeaponBlendAlpha = 0.0f;
	FVector WeaponBlendStartLoc;
	FRotator WeaponBlendStartRot;

	bool bFPSWasActive;
	bool bTPSWasActive;
};
