// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7Npc.h"
#include "K7WeaponsBase.h"
#include "K7WeaponTypes.h" 
#include "K7CombatBase.generated.h"

UCLASS()
class K7_API AK7CombatBase : public AK7Npc
{
    GENERATED_BODY()

public:
    AK7CombatBase();
    UPROPERTY(BlueprintReadOnly, Category = "Combat Stats")
    TArray<AK7WeaponsBase*> Inventory;
protected:
    virtual void BeginPlay() override;

    UPROPERTY()
    

    
    
    FK7RangedWeaponData CurrentRangedData;
    // ************************ reload logic ********************************************
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsReloading = false;
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FTimerHandle ReloadTimerHandle;
    void ReloadCurrentWeapon();
    void OnReloadComplete();
    void CancelReload();
    // **********************************************************************************
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    int32 WeaponType;// 0 it nulll not weapons at all just basic movment, 1 pistols movent of hips and aim, 2 it rifle, in future if be things that need rtranslate we change but now it allready look okay
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    int32 shotPos; //0=no aim no hip jjust free move with weapons or not , 1=hip mena full roattionf of body and strafes, 2=hybrid aim offset and rotate.
    float MaxCarryWeight;
    float CurrentCarryWeight;
    float fireRate;
    int32 secret;
    bool bIsAiming;
    int32 CurrentWeaponIndex;
    void UpdateWeaponData();
    virtual void RepeatFunc();
    virtual void coldown();
    virtual void ResetCombatCooldown();
    virtual void niVezde();
    // Handle used to manage, pause, or clear the timer
    FTimerHandle RepeatingTimerHandle;
    FTimerHandle otherT;
    // Editable property to set the interval time in seconds from the editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Settings")
    float CustomDelayTime;

public:
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    AK7WeaponsBase* CurrentWeapon;

    virtual void Tick(float DeltaTime) override;
    void TryPickupWeapon(const FHitResult& Hit);
    virtual void StartAim();
    virtual void StopAim();
    bool CollectWeapon(AK7WeaponsBase* Weapon);
    void socketstuck(AK7WeaponsBase* W, AK7WeaponsBase* WO);

    bool SwitchWeapon(int Index);

    FHitResult Fire(FVector Start,FVector Direction);
    
};
