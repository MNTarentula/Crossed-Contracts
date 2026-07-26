// Fill out your copyright notice in the Description page of Project Settings.


#include "K7Civilian.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "K7CombatBase.h"
#include "AIController.h"
#include "K7ClothBase.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "ATaskArea.h"
#include <iostream>
#include <algorithm>
#include "Kismet/GameplayStatics.h"
// Sets default values
AK7Civilian::AK7Civilian()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    active = FMath::FRandRange(0.5, 2.5);
    trust = FMath::FRandRange(0.25, 2.0);
	if (active >= 2.f) {
		this->maxSpead = 500.f;
	}
	else if(active >= 1.5f) {
		this->maxSpead = 300.f;
	}
	else {
		this->maxSpead = 125.f;
	}
    toilet = 0.f;
    work = 100.f;
    hung = 0.f;
    toiletP = 3;
    hungP = 2;
    workP = 2;
}

// Called when the game starts or when spawned
void AK7Civilian::BeginPlay()
{
	Super::BeginPlay();
    AICon = Cast<AAIController>(GetController());
    eers();
    time();
    Zapoier();
    TArray<AActor*> FoundActors;

    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AATaskArea::StaticClass(),
        FoundActors
    );
    for (AActor* Actor : FoundActors)
    {
        Areas.Add(Cast<AATaskArea>(Actor));
    }
    
}
void AK7Civilian::randomP(const FVector& Target)
{
    if (!AICon)
        return;

    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

    if (!NavSys)
        return;
    FNavLocation NavPoint;
    bool bFound = NavSys->ProjectPointToNavigation(
        Target,
        NavPoint,
        FVector(200.f, 200.f, 300.f) // Search box
    );
    
    if (bFound)
    {
        PointB = NavPoint.Location;

        AICon->MoveToLocation(PointB);
    }
}
// Called every frame
void AK7Civilian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
    float Dist = FVector::Dist(GetActorLocation(), PointB);

    if (Dist < 50.f && curAreaT == nullptr)
    {
        ft();
    }
    if (scary > 100 * chill) {
        
        if (curAreaT == nullptr || curAreaT->TaskType != ETaskType::Safe) {
            setter(ETaskType::Safe);

            i();
        }
    }

    
    if (!GetWorldTimerManager().IsTimerActive(tt)) {
        if (curAreaT != nullptr) {
            if (FVector::Dist(GetActorLocation(), curAreaT->location) < curAreaT->Radius) {
            
                GetWorldTimerManager().SetTimer(
                tt,
                this,
                &AK7Civilian::ctf,
                2.f,
                true
                );
            }
            
        }
    }
    else {
        if (curAreaT == nullptr) {
            GetWorldTimerManager().ClearTimer(tt);
        }
        if (curAreaT != nullptr) {
            if (FVector::Dist(GetActorLocation(), curAreaT->location) > curAreaT->Radius) {
                GetWorldTimerManager().ClearTimer(tt);
            }
        }
        
    }
    
    FRotator TargetRotation =(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),PointB)- GetActorRotation()).GetNormalized();
    if (currekNpc) {
        TargetRotation = (UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), currekNpc->GetActorLocation()) - GetActorRotation()).GetNormalized();
    }
	TargetRotation.Yaw = FMath::Clamp(TargetRotation.Yaw, -MaxLookAngle, MaxLookAngle);
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -MaxLookAngle, MaxLookAngle);
	TargetRotation.Roll = 0.0f; // Necks don't usually roll much sideways while tracking

	// Smoothly interpolate from current head rotation to the target rotation
	RotatorHead = FMath::RInterpTo(RotatorHead, TargetRotation, DeltaTime, HeadInterpSpeed);
}

void AK7Civilian::eers() {
	
	GetWorldTimerManager().ClearTimer(ers);
	GetWorldTimerManager().SetTimer(
		ers,
		this,
		&AK7Civilian::ok,
		20.f / active, 
		true
	);
}
void AK7Civilian::time() {

    GetWorldTimerManager().ClearTimer(tim);
    GetWorldTimerManager().SetTimer(
        tim,
        this,
        &AK7Civilian::ft,
        2.5f / active, //idk how good is it 5.f it 5 second if yes it need bechange by active if what it time pf look change
        true
    );
}
void AK7Civilian::Zapoier() {

    GetWorldTimerManager().ClearTimer(zapoi);
    GetWorldTimerManager().SetTimer(
        zapoi,
        this,
        &AK7Civilian::needTick,
        20.f, 
        true
    );
}
void AK7Civilian::ft() {
    int32 RandomNum = FMath::RandRange(1, 100);
    PointB = GetActorLocation();
    float max = 1000.f;
    if (RandomNum >= 40) {
        ACharacter* currentC = getNearstNpDir(max, 180.f);
        currekNpc = Cast<AK7Npc>(currentC);
        if (currentC) {
            PointB = currentC->GetActorLocation();
        }
        else {
            FVector NormalizedDirection = GetActorForwardVector().GetSafeNormal();

            // 2. Generate a random distance up to the maximum radius
            float RandomDistance = FMath::FRandRange(0.0f, max);

            // 3. (Optional) Introduce angle variance to create a cone/spread around the direction
            // Set ConeHalfAngleDegrees to 0.0f if you want a strict straight-line variation
            float ConeHalfAngleDegrees = 15.0f;
            FVector RandomizedDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(NormalizedDirection, ConeHalfAngleDegrees);

            // 4. Combine into final world position
            PointB = GetActorLocation() + (RandomizedDirection * RandomDistance);
        }

    }
    else if (RandomNum >= 60 && false) {
        //now it not work becuase fuck i not want add more big function to search object charthers and the other thing good
    }
    else {
        FVector NormalizedDirection = GetActorForwardVector().GetSafeNormal();

        // 2. Generate a random distance up to the maximum radius
        float RandomDistance = FMath::FRandRange(0.0f, max);

        // 3. (Optional) Introduce angle variance to create a cone/spread around the direction
        // Set ConeHalfAngleDegrees to 0.0f if you want a strict straight-line variation
        float ConeHalfAngleDegrees = 15.0f;
        FVector RandomizedDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(NormalizedDirection, ConeHalfAngleDegrees);

        // 4. Combine into final world position
        PointB = GetActorLocation() + (RandomizedDirection * RandomDistance);
    }

    if (currekNpc) {
        int timeScare = currekNpc->wirdo + 0;

        AK7CombatBase* cur = Cast<AK7CombatBase>(currekNpc);
        if (cur) {
            if (cur->CurrentWeapon) {
                timeScare += 100;
                if (cur->cloth) {
                    if (cur->cloth->faction == "civi") {
                        timeScare += 2;
                    }
                    else if (cur->cloth->faction == "poli") {
                        timeScare -= 75;
                    }
                    else if (cur->cloth->faction == "milt") {
                        timeScare -= 70;
                    }
                    else {
                        timeScare += 5;
                    }

                }
                else {
                    timeScare += 2;
                }
            }
        }

        scary += timeScare / trust;
    }
}
void AK7Civilian::setter(ETaskType a) {
    curAreaT = nullptr;
    for (AATaskArea* Area : Areas)
    {
        if (!Area)
            continue;
        if (Area->TaskType == a)
        {

            curAreaT = Area;
            break;
        }
    }
}
void AK7Civilian::ok() {
    
    
    int biggestNeed = -1;
    int secondNeed = -1;
    int toiletScore;
    int hungScore;
    int workScore;
    toiletScore = toilet * (toiletP + FMath::Clamp(toilet / 100, 0, 5));
    hungScore = hung * (hungP + FMath::Clamp(hung / 100, 0, 5));
    workScore = work * (workP + FMath::Clamp(work / 100, 0, 5));
    int biggestScore = -1;
    ETaskType finalTask = ETaskType::None;

    if (toiletScore > biggestScore)
    {
        biggestScore = toiletScore;
        finalTask = ETaskType::Toilet;
    }

    if (hungScore > biggestScore)
    {
        biggestScore = hungScore;
        finalTask = ETaskType::Food;
    }

    if (workScore > biggestScore)
    {
        biggestScore = workScore;
        finalTask = ETaskType::Work;
    }

    if (scary > 100 * chill) {
        finalTask = ETaskType::Safe;

    }
    if (finalTask != ETaskType::None) {

        setter(finalTask);
        if (curAreaT != nullptr) {
            if (curAreaT->TaskType == finalTask) {
                i();
            }
        }
        
        
    }
    //in future it be much more big and strong now it even smaller then ft, because it not fully field and not really change by many factor but like that.
}
void AK7Civilian::i() {
    if (!curAreaT)
        return;
    float Dist = FVector::Dist(GetActorLocation(), PointB);
    float minRename = 5000000.f;
    FVector WP = FVector::ZeroVector;
    AATaskArea* AreaT = nullptr;
    for (AATaskArea* Area : Areas)
    {
        if (!Area)
            continue;
        if (Area->TaskType == curAreaT->TaskType)
        {
            float Distance = FVector::Distance(GetActorLocation(), Area->location);

            if (Distance < minRename)
            {
                minRename = Distance;
                WP = Area->location;
                AreaT = Area;
            }
        }
    }
    if (minRename != 5000000.f) {
        randomP(WP);
        if (AreaT) {
            curAreaT = AreaT;
        }
        

    }
    else if (Dist > 25.f)
    {
        randomP(PointB);
    }
}
void AK7Civilian::ctf() {
    if (curAreaT != nullptr) {
        if (curAreaT->TaskType == ETaskType::Toilet) {
            toilet = FMath::Max(0, toilet - 10);
        }
        if (curAreaT->TaskType == ETaskType::Food) {
            hung = FMath::Max(0, hung - 2.5);
        }
        if (curAreaT->TaskType == ETaskType::Work) {
            work = FMath::Max(0, work - 0.1);
        }
    }
}
void AK7Civilian::needTick() {
    toilet+=2;
    hung+=2;
}
ACharacter* AK7Civilian::getNearstNpDir(float MaxRange, float MaxAngleDegrees) {
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // or ECC_WorldDynamic etc.

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this); // Ignore self

    TArray<AActor*> OverlappingActors;

    // 2. Scan in a spherical range
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        MaxRange,
        ObjectTypes,
        ACharacter::StaticClass(), // Filter by your class
        IgnoredActors,
        OverlappingActors
    );

    ACharacter* NearestNPC = nullptr;
    float NearestDistanceSquared = MAX_flt;

    // Get your forward vector and current location
    FVector ForwardDir = GetActorForwardVector();
    FVector CurrentLoc = GetActorLocation();

    // 3. Filter by Direction and Find Nearest
    for (AActor* Actor : OverlappingActors)
    {
        ACharacter* NPC = Cast<ACharacter>(Actor);
        if (NPC)
        {
            FVector DirToNPC = NPC->GetActorLocation() - CurrentLoc;
            float DistanceSquared = DirToNPC.SizeSquared();

            // Skip if it's further away than our current known nearest
            if (DistanceSquared > NearestDistanceSquared)
            {
                continue;
            }

            // Direction calculation using Dot Product
            DirToNPC.Normalize();
            float DotProduct = FVector::DotProduct(ForwardDir, DirToNPC);

            // Convert angle to threshold: cos(Angle)
            float AngleThreshold = FMath::Cos(FMath::DegreesToRadians(MaxAngleDegrees));

            if (DotProduct >= AngleThreshold)
            {
                NearestNPC = NPC;
                NearestDistanceSquared = DistanceSquared;
            }
        }
    }

    return NearestNPC;
}


