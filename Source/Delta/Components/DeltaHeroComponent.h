// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "DeltaHeroComponent.generated.h"

class UDeltaInputConfig;
class UInputMappingContext;

UCLASS(MinimalAPI, Blueprintable, Meta = (BlueprintSpawnableComponent))
class UDeltaHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeltaHeroComponent(const FObjectInitializer& ObjectInitializer);

	// Call once the input component and subsystem are ready
	void InitializePlayerInput(class UInputComponent* PlayerInputComponent);

	UPROPERTY(EditAnywhere, Category = "Delta|Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultInputMappings;

	UPROPERTY(EditAnywhere, Category = "Delta|Input")
	TObjectPtr<UDeltaInputConfig> InputConfig;

	// Find the HeroComponent on an actor; may be on the avatar actor or the owning player state
	UFUNCTION(BlueprintCallable, Category = "Delta|Hero")
	static UDeltaHeroComponent* FindHeroComponent(const AActor* Actor);

protected:
	virtual void BeginPlay() override;

	// Input handlers
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_HoldCrouch_Pressed(const FInputActionValue& InputActionValue);
	void Input_HoldCrouch_Released(const FInputActionValue& InputActionValue);
	void Input_AutoRun(const FInputActionValue& InputActionValue);
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

private:
	APawn* GetPawn() const;
};
