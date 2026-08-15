// Fill out your copyright notice in the Description page of Project Settings.


#include "K7CombatBase.h"
#include "K7RangedWeapons.h"
#include "DrawDebugHelpers.h"
#include "PistolCOM.h"
#include "Engine/World.h"
#include "K7ClothBase.h"
#include "RadioItem.h"
AK7CombatBase::AK7CombatBase()
{
    PrimaryActorTick.bCanEverTick = true;
    secret = 0;
    CurrentWeapon = nullptr;
    CustomDelayTime = 20.0f;
    fireRate = 0.f;
    MaxCarryWeight = 50.0f;
    CurrentCarryWeight = 0.0f;
    shotPos = 0;
    bIsAiming = false;
    CurrentWeaponIndex = 0;
}

void AK7CombatBase::BeginPlay()
{
    Super::BeginPlay();
    
}
void AK7CombatBase::ResetCombatCooldown()
{
    // Clear any existing countdown so it doesn't trigger prematurely
    GetWorldTimerManager().ClearTimer(RepeatingTimerHandle);
    // Start a fresh, non-looping (false) countdown
    GetWorldTimerManager().SetTimer(
        RepeatingTimerHandle,
        this,
        &AK7CombatBase::RepeatFunc,
        CustomDelayTime,
        false
    );
    
}
void AK7CombatBase::niVezde()
{
    GetWorldTimerManager().ClearTimer(otherT);
    GetWorldTimerManager().SetTimer(
        otherT,
        this,
        &AK7CombatBase::coldown,
        fireRate,
        false
    );
    secret = 1;
}
void AK7CombatBase::RepeatFunc()
{

    // Your logic goes here
    shotPos = 0;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Function executed successfully!"));
    }
}
void AK7CombatBase::coldown()
{

    // Your logic goes here
    
    secret = 0;
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Function executed successfully!"));
    }
}

void AK7CombatBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void AK7CombatBase::StartAim()
{

    shotPos = 2;
}

void AK7CombatBase::StopAim()
{
    shotPos = 1;
}
void AK7CombatBase::UpdateWeaponData()
{
    if (!CurrentWeapon)
    {
        // No weapon equipped? Zero out the data struct safely
        CurrentRangedData = FK7RangedWeaponData();
        WeaponType = 0;
        shotPos = 0;
        secret = 0;
        CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);

        return;
    }
    CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
    // Try to cast the base pointer to a Ranged Weapon pointer
    AK7RangedWeapons* RangedWeapon = Cast<AK7RangedWeapons>(CurrentWeapon);

    if (RangedWeapon)
    {
        // Cast Succeeded! This is a gun. Fetch its struct data.
        CurrentRangedData = RangedWeapon->GetWeaponData();
        WeaponType = CurrentRangedData.weaponType;
        fireRate = 60.0f / CurrentRangedData.FireRate;
        UE_LOG(LogTemp, Log, TEXT("Equipped Ranged Weapon: %s"), *CurrentRangedData.Name);
    }
    else
    {
        // Cast Failed! It's likely a melee weapon or special weapon.
        // Clear the ranged data structure so old gun stats don't stick around.
        WeaponType = 0;
        CurrentRangedData = FK7RangedWeaponData();
    }
}
bool AK7CombatBase::CollectWeapon(
    AK7WeaponsBase* Weapon
)
{
    if (!Weapon)
    {
        return false;
    }
    CancelReload();
    if (!Inventory.Contains(Weapon))
    {
        Inventory.Add(Weapon);
    }

    
    CurrentWeapon = Weapon;
    UpdateWeaponData();
        
    

    return true;
}

bool AK7CombatBase::SwitchWeapon(
    int Index
)
{
    if (CurrentWeaponIndex == Index)
    {
        return false;
    }
    CancelReload();

    if (!Inventory.IsValidIndex(Index))
    {
        socketstuck(nullptr,CurrentWeapon);
        CurrentWeapon = nullptr;
        UpdateWeaponData();
        
        return false;
    }
    
    AK7WeaponsBase* WO = CurrentWeapon;
    CurrentWeapon = Inventory[Index];
    socketstuck(Inventory[Index], WO);
    UpdateWeaponData();
    APistolCOM* ws = Cast<APistolCOM>(Inventory[Index]);
    if (ws) { ws->MeshComponent->SetHiddenInGame(false); }
    return true;
}

FHitResult AK7CombatBase::Fire(FVector Start, FVector Direction)
{
    FHitResult Hit;

    if (!CurrentWeapon) return Hit;
    
    // 1. Cast to check if it's a gun, so we can look at the REAL-TIME ammo
    AK7RangedWeapons* RangedWeapon = Cast<AK7RangedWeapons>(CurrentWeapon);
    if (RangedWeapon)
    {
        
        // If the gun on the actor has 0 or fewer bullets, stop the trace immediately!
        if (RangedWeapon->GetWeaponData().CurrentAmmo <= 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("Click! Out of ammo. Need to reload."));
            return Hit;
        }
        // 2. Trigger the shot! (This drops the gun's ammo by 1)
        CurrentWeapon->Use();

        // 3. CRITICAL: Refresh our local character copy so the UI/State updates perfectly
        UpdateWeaponData();

        // 4. Trace Spawner pays off using the freshly synced range data
        if (CurrentRangedData.Range > 0.0f)
        {
            float CurrentSpread = shotPos == 2 ? CurrentRangedData.aimSpeard: CurrentRangedData.hipSpeard;
            Direction = FMath::VRandCone(Direction, CurrentSpread);
            FVector End = Start + (Direction * CurrentRangedData.Range);

            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(this);
            QueryParams.AddIgnoredActor(CurrentWeapon);

            GetWorld()->LineTraceSingleByChannel(
                Hit,
                Start,
                End,
                ECC_Visibility,
                QueryParams
            );

            DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 2.0f);
        }
    }
    return Hit;
}

// Add this function to your CombatBase class so players/NPCs can trigger a reload!
void AK7CombatBase::ReloadCurrentWeapon()
{
    if (!CurrentWeapon || bIsReloading) return;

    AK7RangedWeapons* RangedWeapon = Cast<AK7RangedWeapons>(CurrentWeapon);
    // Make sure the gun actually needs ammo before reloading!
    if (RangedWeapon)
    {
        if (CurrentRangedData.CurrentAmmo < CurrentRangedData.Ammo) {
            bIsReloading = true;

            // 1. Play your animations here (we will cover this next)
            //gemini you idiot from where you get a idea that play animation it function in c++ and not fucking event graph and animation graph shit based on bIsRealoading hahahahahah

            // 2. Start the timer using the weapon's specific reload duration
            float ReloadTime = CurrentRangedData.realoadT; // Add a float ReloadTime to your weapon class! here agin miss but no worry reload time for pistol 3.f okay??

            GetWorld()->GetTimerManager().SetTimer(
                ReloadTimerHandle,
                this,
                &AK7CombatBase::OnReloadComplete,
                ReloadTime,
                false
            );
        }
    }
}

void AK7CombatBase::OnReloadComplete()
{
    bIsReloading = false;

    AK7RangedWeapons* RangedWeapon = Cast<AK7RangedWeapons>(CurrentWeapon);
    if (RangedWeapon)
    {
        RangedWeapon->Reload(); // Actually refills the ammo variables wow here you got right thing firt time nice gemini sorry for bully but hahahahha
        UpdateWeaponData();
        UE_LOG(LogTemp, Log, TEXT("Reload Complete! Ammo Refilled."));
    }
}

void AK7CombatBase::CancelReload()
{
    if (bIsReloading)
    {
        // Stop the timer dead in its tracks so OnReloadComplete never fires okay now i suppus to add us of that function on switching on fire and on other shit yes?
        GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
        bIsReloading = false;

        // (Optional) Stop character montage here if needed
        UE_LOG(LogTemp, Log, TEXT("Reload Canceled!"));
    }
}
void AK7CombatBase::TryPickupWeapon(const FHitResult& Hit)
{
    // Trace
    /*FVector End = Start + (Direction * range);
    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    if (cloth)
    {
        QueryParams.AddIgnoredActor(cloth);
    }
    FCollisionShape MySphere = FCollisionShape::MakeSphere(250.0f);

    GetWorld()->SweepSingleByChannel(
        Hit,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        MySphere,
        QueryParams
    );*/
    // Check if hit weapon
    if (Hit.bBlockingHit && Hit.GetActor())
    {
        
        // Keep your awesome tracking debug logs intact!
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s | Component: %s"), *Hit.GetActor()->GetName(), *Hit.GetComponent()->GetName());
        UE_LOG(LogTemp, Warning, TEXT("Hit Bone: %s"), *Hit.BoneName.ToString());
        // 4. Check if we shot an NPC base target

        AK7WeaponsBase* W = Cast<AK7WeaponsBase>(Hit.GetActor());
        AK7WeaponsBase* WO = CurrentWeapon;
        CollectWeapon(W);
        socketstuck(W, WO);
        APistolCOM* ws = Cast<APistolCOM>(W);
        if (ws) { ws->MeshComponent->SetHiddenInGame(false); }
    }
    
}
void AK7CombatBase::socketstuck(AK7WeaponsBase* W, AK7WeaponsBase* WO) {
    // tarn fuck off no chance you understand that, it i make for you, if you read that mean you try understand, LOSER FUCK OFF
    if (WO == W)
    {
        return;
    }
    if (WO) {
        AK7RangedWeapons* wsd = Cast<AK7RangedWeapons>(WO);
        if (wsd) {
            FK7RangedWeaponData old = wsd->GetWeaponData();
            if (old.weaponType>1) {

                FAttachmentTransformRules AttachmentRules(
                    EAttachmentRule::SnapToTarget, // Location: Snap to back
                    EAttachmentRule::SnapToTarget, // Rotation: Snap to back
                    EAttachmentRule::KeepRelative, // Scale: KEEP THE GUN'S ORIGINAL SIZE!
                    false                          // Weld simulated bodies
                );
                FName socket = TEXT("WeaponBack");
                wsd->AttachToComponent(GetMesh(), AttachmentRules, socket);
                
            }
            else if(old.weaponType==1){
                APistolCOM* ws = Cast<APistolCOM>(wsd);
                if (ws) { ws->MeshComponent->SetHiddenInGame(true);}
            }
        }
    }
    if (W)
    {// CollectWeapon()
        UE_LOG(LogTemp, Warning, TEXT("Try pick up 7"));
        
        
        W->SetActorEnableCollision(false);
        /*APistolCOM* ws = Cast<APistolCOM>(W);
        if (ws) { ws->Physic(false); } current not working in future will be fixed and add for evrythings*/

        // Attach weapon
        // that copy from other part of code i cant but i can check the og of the unreal source docmentry like real program wait a second
        FAttachmentTransformRules AttachmentRules(
            EAttachmentRule::SnapToTarget, // Location: Snap to hand
            EAttachmentRule::SnapToTarget, // Rotation: Snap to hand
            EAttachmentRule::KeepRelative, // Scale: KEEP THE GUN'S ORIGINAL SIZE!
            false                          // Weld simulated bodies
        );
        FName socket = TEXT("Weaponsocket");
        W->AttachToComponent(GetMesh(), AttachmentRules, socket);
        UE_LOG(LogTemp, Warning, TEXT("Try pick up 8"));
        
    }
}
float AK7CombatBase::getInfoById(int32 id) {// in future if had radio you will can use it as killer and as gaurd killer only hear and gaurd use also
    if (radioCur) {
        return radioCur->getInfoById(id);
    }
    return 0.f;
}