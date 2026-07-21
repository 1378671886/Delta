// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "DeltaPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UInv_InventoryComponent;

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

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventory();

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void SetupInputComponent() override;

	void ProcessAutoRun(float DeltaTime);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void PrimaryInteract();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InventoryIMC;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PrimaryInteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleInventoryAction;

private:
	bool bIsAutoRunning = false;
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;

};
