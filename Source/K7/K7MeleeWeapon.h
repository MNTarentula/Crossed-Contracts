// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7WeaponsBase.h"
#include "K7MeleeWeapon.generated.h"

UCLASS()
class K7_API AK7MeleeWeapon : public AK7WeaponsBase
{
	GENERATED_BODY()

public:
	AK7MeleeWeapon();

protected:
	UPROPERTY(EditAnywhere)
	float Reach;

public:
	/*virtual void Use() override;*/
};
