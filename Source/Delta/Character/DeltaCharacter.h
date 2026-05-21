// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"
#include "DeltaCharacter.generated.h"

class UDeltaAbilitySystemComponent;
class UDeltaEquipmentManagerComponent;
class UDeltaHeroComponent;
class UDeltaCharacterMovementComponent;
class UDeltaPawnData;

UCLASS(MinimalAPI, Config = Game)
class ADeltaCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ADeltaCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Delta|Character")
	UDeltaAbilitySystemComponent* GetDeltaAbilitySystemComponent() const;

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	//~IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~End of IGameplayTagAssetInterface

	UFUNCTION(BlueprintCallable, Category = "Delta|Character")
	void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = "Delta|Character")
	bool TryInteract();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Delta|Character")
	TObjectPtr<UDeltaEquipmentManagerComponent> EquipmentManagerComponent;

protected:
	//~AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor

	//~APawn
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;
	//~End of APawn

	//~ACharacter
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual bool CanJumpInternal_Implementation() const override;
	//~End of ACharacter

	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);
	void InitializeGameplayTags();
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Delta|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDeltaHeroComponent> HeroComponent;

	// Pawn data used to grant abilities and configure input for this character.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Delta|Pawn", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<const UDeltaPawnData> PawnData;
};
