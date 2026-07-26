// Fill out your copyright notice in the Description page of Project Settings.


#include "AsultAvtomatK.h"

// Sets default values
AAsultAvtomatK::AAsultAvtomatK()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	WeaponData.Name = TEXT("Ak 47!");
	WeaponData.weaponType = 2;
	WeaponData.Damage = 45.0f;
	WeaponData.Ammo = 30;
	WeaponData.CurrentAmmo = WeaponData.Ammo;
	WeaponData.EffB = 1;
	WeaponData.EffS = 3;
	WeaponData.FireRate = 750;
	WeaponData.JamChance = 1.f / 200000.f;
	WeaponData.Range = 30000.0f; //300 meters
	WeaponData.TraceType = 0;
	WeaponData.Weight = 5.f;
	WeaponData.realoadT = 5.f;
	WeaponData.hipSpeard = 0.2f;
	WeaponData.aimSpeard = 0.05f;

}

// Called when the game starts or when spawned
void AAsultAvtomatK::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent = FindComponentByClass<USkeletalMeshComponent>();
}


AK7WeaponsBase* AAsultAvtomatK::taked()
{
	// Default behavior if a child doesn't override it
	UE_LOG(LogTemp, Warning, TEXT("Base Weapon Used"));
	return this;
}

void AAsultAvtomatK::Physic(bool s)
{
	UE_LOG(LogTemp, Warning, TEXT("Try pick up 213??"));
	if (MeshComponent != nullptr)
	{
		MeshComponent->SetSimulatePhysics(s);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CRASH PREVENTED: MeshComponent is NULL inside APistolCOM!"));
	}
}


