// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeltaEquipmentManagerComponent.generated.h"

class AWeapon;
class UDeltaAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DELTA_API UDeltaEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDeltaEquipmentManagerComponent();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void EquipWeapon(AWeapon* Weapon);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Equipment")
	void UnequipWeapon(AWeapon* Weapon);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	TArray<AWeapon*> GetEquippedWeapons() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool TryPickUpWeapon();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	bool HasNearbyWeapon() const;


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<TObjectPtr<AWeapon>> EquippedWeapons;

	UDeltaAbilitySystemComponent* GetAbilitySystemComponent() const;
};
