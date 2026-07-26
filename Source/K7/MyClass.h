// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class K7_API MyClass
{
public:
	MyClass();
	~MyClass();
	struct FRangedWeaponData { 
		int TraceType; float Damage; FString Name; float JamChance; int Ammo; float Recoil; float FireRate; float EffB; float EffS; float Range; float Weight; 
	};
};
