// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DeltaPlayerState.generated.h"

class UDeltaAbilitySystemComponent;
class UDeltaPawnData;

UCLASS(MinimalAPI, Config = Game)
class ADeltaPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADeltaPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Delta|PlayerState")
	UDeltaAbilitySystemComponent* GetDeltaAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Sets the pawn data and grants its ability sets to the ASC.
	void SetPawnData(const UDeltaPawnData* InPawnData);

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

protected:
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PawnData();

private:
	UPROPERTY(VisibleAnywhere, Category = "Delta|PlayerState")
	TObjectPtr<UDeltaAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UDeltaPawnData> PawnData;
};
