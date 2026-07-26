#pragma once

#include "CoreMinimal.h"
#include "K7WeaponsBase.h"
#include "K7WeaponTypes.h" // Crucial: Includes your new struct!
#include "K7RangedWeapons.generated.h"

UCLASS()
class K7_API AK7RangedWeapons : public AK7WeaponsBase
{
	GENERATED_BODY()

public:
	AK7RangedWeapons();

	// We override the father's Use function to make guns shoot
	virtual void Use() override;
	virtual void Physic(bool s);
	void Reload();
	
	// Simple getter function so your Character can grab the whole struct at once
	UFUNCTION(BlueprintCallable, Category = "Weapon Data")
	FK7RangedWeaponData GetWeaponData() const { return WeaponData; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Config")
	FK7RangedWeaponData WeaponData;


};