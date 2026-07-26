#include "PistolCOM.h"

APistolCOM::APistolCOM()
{
	

	// 2. This sets the data stats for this specific gun
	WeaponData.Name = TEXT("Glock 17");
	WeaponData.weaponType = 1;
	WeaponData.Damage = 28.0f;
	WeaponData.Ammo = 10;
	WeaponData.CurrentAmmo = WeaponData.Ammo;
	WeaponData.EffB = 1;
	WeaponData.EffS = 1;
	WeaponData.FireRate = 200;
	WeaponData.JamChance = 1.f / 10000.f;
	WeaponData.Range = 7500.0f; //75 meters
	WeaponData.TraceType = 0;
	WeaponData.Weight = 0.95f;
	WeaponData.realoadT = 3.f;
	WeaponData.hipSpeard = 0.08f;
	WeaponData.aimSpeard = 0.01f;
	
}
AK7WeaponsBase* APistolCOM::taked()
{
	// Default behavior if a child doesn't override it
	UE_LOG(LogTemp, Warning, TEXT("Base Weapon Used"));
	return this;
}
void APistolCOM::BeginPlay()
{
	Super::BeginPlay();

	// The Blueprint components exist now! We can safely grab it.
	MeshComponent = FindComponentByClass<USkeletalMeshComponent>();
}
void APistolCOM::Physic(bool s)
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
