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
#include "K7WeaponsBase.h"
#include "DmgDealer.h"
// Sets default values
AK7Civilian::AK7Civilian()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    
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
    if (manger) {
        idsMem.SetNumZeroed(manger->IDcur);
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

    if (Dist < 50.f && (curAreaT == nullptr || curAreaT->TaskType == ETaskType::None))// if not had a task just not stop wander and find new thing to look at
    {
        ft();
    }
    if (scary > 100 * chill) {//it scary setter and maker, if currentlly the npc is over the threshold he start run to safe place, because seacurity and safe of the person is reflex and after go the task and other thinks
        
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

void AK7Civilian::eers() {// timer setter for the ok()
	
	GetWorldTimerManager().ClearTimer(ers);
	GetWorldTimerManager().SetTimer(
		ers,
		this,
		&AK7Civilian::ok,
		20.f / active, 
		true
	);
}
void AK7Civilian::time() {// timer setter for the ft()

    GetWorldTimerManager().ClearTimer(tim);
    GetWorldTimerManager().SetTimer(
        tim,
        this,
        &AK7Civilian::ft,
        2.5f / active, //idk how good is it 5.f it 5 second if yes it need bechange by active if what it time pf look change
        true
    );
}
void AK7Civilian::Zapoier() {// timer setter for the needs.

    GetWorldTimerManager().ClearTimer(zapoi);
    GetWorldTimerManager().SetTimer(
        zapoi,
        this,
        &AK7Civilian::needTick,
        20.f, 
        true
    );
}
void AK7Civilian::ft() {// first think it faster then ok happend and it just anlyze current world around decicde what to look and the emrgnsy fealling not toilet not work, just if scary,ivistigate or take health care.

    int32 RandomNum = FMath::RandRange(1, 100);
    PointB = GetActorLocation();
    float max = 1000.f;
    
        AActor* currentC = whatMostInterstT(max, 180.f);
        currekNpc = Cast<AK7Npc>(currentC);
        if (currentC) {
            PointB = currentC->GetActorLocation();
        }
        else { // random if nothing intersting got find to watch.
            AActor* planB = getNearstNpDir(max, 180.f);
            currekNpc = Cast<AK7Npc>(planB);
            if (planB) {
                PointB = planB->GetActorLocation();
            }
            else {
                FVector NormalizedDirection = GetActorForwardVector().GetSafeNormal();
                float RandomDistance = FMath::FRandRange(0.0f, max);
                float ConeHalfAngleDegrees = 15.0f;
                FVector RandomizedDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(NormalizedDirection, ConeHalfAngleDegrees);
                PointB = GetActorLocation() + (RandomizedDirection * RandomDistance);
            }
            
            
        }
    //okay idea is basic as h*** do just like scary grow to interest also based on traits, and do new invstigate emrgnsy in the tick and add a task dec to ok func

    
    if (currekNpc) { // it scary part, it just thing bassed on people the npc current look at, if he look at one but he got shoted scary not go up but that gonna be added soon
        int timeScare = currekNpc->wirdo + 0;
        int Timeinters = idsMem[currekNpc->ObId] + currekNpc->wirdo/ 2;

        AK7CombatBase* cur = Cast<AK7CombatBase>(currekNpc);
        if (cur) { // need more interst change by cloth but basiclly what now is okay.
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
                    Timeinters += 60;//if had weapon, and had no cloth what the hell??
                }
            }
        }
        if (currekNpc->cloth) {
            // if had cloth basic npc interst grow not had time for that now.
        }
        else {
            Timeinters += 60;// yeah can be count 2 time but if you no cloth and weapon handler ++
        }
        interst = Timeinters * dec;
        if (Timeinters > 100 / dec) {
            if (dec <= 1.45) {
                dec += 0.05; // yeah it trait but it mood trait even not dec can be dec in time if he see very many intersting things likeman with pistol withhout cloth and weapons bodies and more
            }
        }
        scary += timeScare / trust;
    }
    else {
        if (currentC) {
            int curId=0;
            AK7WeaponsBase* weap = Cast<AK7WeaponsBase>(currentC);
            if (weap) {
                curId = weap->ObId;
            }
            AK7ClothBase* clo = Cast<AK7ClothBase>(currentC);
            if (clo) {
                curId = clo->ObId;
            }
            interst = idsMem[curId];
        }
        
    }
}
void AK7Civilian::setter(ETaskType a) {// just a setter of current type of task, not to where go, it set what to do by the input.
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
void AK7Civilian::ok() { // it function of hard thinking that decide by current needs and prority of them body what task currentlly more importnat.
    
    
    //int biggestNeed = -1;
    //int secondNeed = -1;
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
void AK7Civilian::i() { // i it excution of the body it chosse nearest point of the task and go to them, that function not decide it do.
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
void AK7Civilian::ctf() {// called evry 2 seconds, but only where in zone of task and had that task cant be in toilet and do the task of food. in tick main logic of check that.
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
void AK7Civilian::needTick() { // it update the needs evry 20 seceonds by timer zapoier or like that
    toilet+=4;
    hung+=4;
}
AActor* AK7Civilian::whatMostInterstT(float MaxRange, float MaxAngleDegrees) {
    AActor* mostHave = nullptr;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this); 

    TArray<AActor*> OverlappingActors;
    TArray<AActor*> taggedOnes;
    bool bFoundActors = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        MaxRange,
        ObjectTypes,
        AActor::StaticClass(),
        IgnoredActors,
        OverlappingActors
    );
    if (bFoundActors) {
        for (AActor* Actor : OverlappingActors)
        {
            if (Actor && Actor->ActorHasTag(FName("interst")))
            {
                taggedOnes.Add(Actor);
            }
        }
    }
    
    int32 maximums =-1;

    for (AActor* Actor : taggedOnes)
    {
        int32 curId = 0;
        int32 currentI=-1;
        AK7Npc* NPC = Cast<AK7Npc>(Actor);
        if (NPC) {
            curId = NPC->ObId;
            currentI = NPC->interesting + NPC->GetVelocity().Size()/20+NPC->wirdo;//based on base interst,on velocity if the man around you fly he may take your attetion, and how sus he look (wirdo) also need count how naer you is it.
            if (NPC->cloth) {
                AK7ClothBase* fast = NPC->cloth;
                if (fast->faction == FString("poli")) {
                    currentI += 20;
                }
                else if (fast->faction == FString("milt")) {//not set now not imporntat
                    currentI += 35;
                }
                currentI += fast->blood*5;
                currentI += fast->holesBreaks * 2.5;
            }
            else {
                currentI += 50; //no cloth it verry intersting
            }
            if (NPC->sol <= 1) {
                currentI += 100;
            }
            AK7CombatBase* wNp = Cast<AK7CombatBase>(NPC);
            if (wNp) {
                if (wNp->CurrentWeapon) {
                    currentI += 120;
                }
                
                for (AK7WeaponsBase* w12 : wNp->Inventory) {
                    if (w12->MeshComponent->GetVisibleFlag()) {
                        if (w12 != wNp->CurrentWeapon) {
                            currentI += 70;
                        }                       
                    }
                }
            }
        }
        AK7WeaponsBase* weap = Cast<AK7WeaponsBase>(Actor);
        if (weap) { 
            curId = weap->ObId;
            currentI = weap->interesting / chill;
        }
        AK7ClothBase* clo = Cast<AK7ClothBase>(Actor);
        if (clo) {
            curId = clo->ObId;
            currentI = clo->interesting;
            if(clo->faction == FString("poli")) {
                currentI += 50;
            }else  if(clo->faction == FString("milt")) {
                currentI += 60;
            }
            currentI += clo->blood * 7;
            currentI += clo->holesBreaks * 3.5;
        }
        float dis = FVector::Distance(GetActorLocation(), Actor->GetActorLocation());
        currentI += (MaxRange - dis) / 5;
        if (idsMem[curId] >= currentI) {
            idsMem[curId] = currentI;
            if (currentI > maximums) {
                maximums = currentI;
                mostHave = Actor;
            }
        }
        
    }
    return mostHave;
}
ACharacter* AK7Civilian::getNearstNpDir(float MaxRange, float MaxAngleDegrees) {
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); 

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this); // that the npc not watch at him self 

    TArray<AActor*> OverlappingActors;

    // 2. Scan in a spherical range for find object that kind of Characters
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        MaxRange,
        ObjectTypes,
        ACharacter::StaticClass(), // Filter by Character
        IgnoredActors,
        OverlappingActors
    );

    ACharacter* NearestNPC = nullptr;
    float NearestDistanceSquared = MAX_flt;

    // Get your forward vector and current location
    FVector ForwardDir = GetActorForwardVector();
    FVector CurrentLoc = GetActorLocation();

    // 3. Filter by Direction and Find Nearest character
    for (AActor* Actor : OverlappingActors)
    {
        ACharacter* NPC = Cast<ACharacter>(Actor);
        if (NPC)
        {
            FVector DirToNPC = NPC->GetActorLocation() - CurrentLoc;
            float DistanceSquared = DirToNPC.SizeSquared();

            // Skip if it's further away than our current known nearest basic optimzation
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


