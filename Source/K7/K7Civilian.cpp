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
#include "K7InterstAct.h"
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
    }else if (interst > 100 && (currekNpc || currekAct) && !investig.Active) {//it interst setter
        // logic when the civi interst in go to invastigate what is going on here.
        strInvestg();
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
    currekNpc = nullptr;
    currekAct = nullptr;
    int32 RandomNum = FMath::RandRange(1, 100);
    PointB = GetActorLocation();
    float max = 1000.f;
    
        AActor* currentC = whatMostInterstT(max, 180.f);
        currekNpc = Cast<AK7Npc>(currentC);
        currekAct = Cast<AK7InterstAct>(currentC);
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

//************************************************ invastigate functions ***************************************************************

void AK7Civilian::strInvestg() {//it called in tick when trsh hold over, it start the actions of invastigate 
    investig = FInvestigationContext();
    investig.Active = true;
    if (IsValid(currekNpc)) {
        investig.TriggerActor = currekNpc;
    }
    else if (IsValid(currekAct)) {
        investig.TriggerActor = currekAct;
    }
    if (investig.TriggerActor) {
        investig.SceneCenter = investig.TriggerActor->GetActorLocation();
        investig.timeOfInvest = GetWorld()->GetTimeSeconds();
        investig.evidAct.Add(investig.TriggerActor);
        investig.timeEvidFind.Add(investig.timeOfInvest);
        investig.InterStats = interst;
    }

    decInvestAc();
}


void AK7Civilian::updInvestg() {
    // it currently full upd
    bool check = true;
    if (IsValid(currekNpc)) {
        for (AActor*& np : investig.evidAct)
        {
            if (np == currekNpc) {
                check = false;
                break;
            }
        }
        if (check) {
            investig.evidAct.Add(currekNpc);
            investig.timeEvidFind.Add(GetWorld()->GetTimeSeconds());
            investig.InterStats += interst / 2;
            investig.locationsFi.Add(currekNpc->GetActorLocation());
            decInvestAc();
        }
    }
    else if (IsValid(currekAct)) {
        for (AActor*& np : investig.evidAct)
        {
            if (np == currekAct) {
                check = false;
                break;
            }
        }
        if (check) {
            investig.evidAct.Add(currekAct);
            investig.timeEvidFind.Add(GetWorld()->GetTimeSeconds());
            investig.InterStats += interst / 2;
            investig.locationsFi.Add(currekAct->GetActorLocation());
            decInvestAc();
        }
        
    }
}
void AK7Civilian::updTheo(int32 sucs,int32 tri) {

    if (tri == 0 || sucs == 0)
    {
        return;
    }

    if (investig.curTri == 0)
    {
        investig.curTri = tri;
        investig.triSucs = sucs;
        return;
    }

    if (investig.curTri == tri)
    {
        investig.triSucs += sucs;
    }
    else if (sucs > investig.triSucs)
    {
        investig.curTri = tri;
        investig.triSucs = sucs;
    }
    else
    {
        investig.triSucs -= sucs / 3;
    }

}

void AK7Civilian::decInvestAc() {
    const int32 Count = investig.evidAct.Num();
    if (Count == 0)
    {
        CurInvesActi = EInvestigationAction::LookAround;
        return;
    }

    const float Now = GetWorld()->GetTimeSeconds();

    float SequenceScore = 0.f;
    float DangerScore = 0.f;
    float HelpScore = 0.f;
    float SuspicionScore = 0.f;
    // 1 it be dead or no sense,2 pistol on floor,3 injured or crawling, 4 suspicons man, 5 strange sound, 6 draged man,7 possible murder, 8 possible stealing of cloth, 9 the suspect is murder!
    int32 curTri = 0; // same as fir but current and not started teahory randlor intagrated
    int32 triSucs = 0;// 0-100 it how he sure he right if he be sure more then 90 changes by dec and chill
    for (int32 i = investig.xue; i < Count; ++i)
    {
        AActor* Ev = investig.evidAct[i];
        if (!IsValid(Ev))
        {
            continue;
        }

        const float FoundTime = investig.timeEvidFind.IsValidIndex(i) ? investig.timeEvidFind[i] : Now;

        const FVector FoundLoc = investig.locationsFi.IsValidIndex(i) ? investig.locationsFi[i] : Ev->GetActorLocation();

        const float Age = Now - FoundTime;
        const float DistFromScene = FVector::Dist(FoundLoc, investig.SceneCenter);

        // older from found time evidence matters less
        float AgeWeight = 1.f;
        if (Age > 60.f) AgeWeight = 0.4f;
        else if (Age > 30.f) AgeWeight = 0.6f;
        else if (Age > 10.f) AgeWeight = 0.8f;

        // far away evidence matters less
        float DistWeight = 1.f;
        if (DistFromScene > 1000.f) DistWeight = 0.2f;
        else if (DistFromScene > 300.f) DistWeight = 0.5f;
        else if (DistFromScene > 100.f) DistWeight = 0.8f;

        const float EvidenceWeight = AgeWeight * DistWeight;
        const float Score = EvidenceWeight * 10.f;
        SequenceScore += Score;
        if (Ev == investig.TriggerActor)
        {
            if (AK7Npc* N = Cast<AK7Npc>(Ev))
            {
                const float Speed = N->GetVelocity().Size();

                if (N->sol <= 1)
                {
                    investig.firTri = 1;

                    HelpScore += 15.f;
                    SuspicionScore += 15.f;

                    // A moving unconscious/dead actor may mean dragging or physics movement. 
                    if (Speed > 80.f)
                    {
                        investig.firTri = 6;
                        SuspicionScore += 25.f;
                        DangerScore += 8.f;
                        triSucs += 15;
                    }
                    int32 diff = FVector::Dist(N->GetActorLocation(), investig.SceneCenter);;
                    if (diff > 9) { //another check that it moved. or dragged from a place that he was found.
                        investig.firTri = 6;
                        SuspicionScore += 25.f;
                        DangerScore += 8.f;

                    }
                    continue;
                }
                else if (N->sol < 4)
                {
                    investig.firTri = 3;

                    HelpScore += 17.f;
                    SuspicionScore += 18.f;

                    if (Speed > 250.f)
                    {
                        // Injured person moving quickly may be fleeing danger.
                        SuspicionScore += 12.f;
                        DangerScore += 6.f;
                    }
                    if (N->wirdo + idsMem[N->ObId] / 2.f >= 100.f * trust) {
                        investig.curSuspect = N;
                        investig.firTri = 4;
                        SuspicionScore += N->wirdo;
                        SuspicionScore += idsMem[N->ObId] / 2.f;
                        if (Speed > 400.f)
                        {
                            SuspicionScore += 18.f;
                            triSucs += 8;
                        }
                    }
                    continue;
                }

                investig.curSuspect = N;
                investig.firTri = 4;

                SuspicionScore += N->wirdo;
                SuspicionScore += idsMem[N->ObId] / 2.f;

                if (Speed > 400.f)
                {
                    SuspicionScore += 18.f;
                    triSucs += 8;
                }

                if (N->wirdo > 30.f)
                {
                    triSucs += 10;
                }

                continue;
            }

            if (AK7WeaponsBase* Weapon = Cast<AK7WeaponsBase>(Ev))
            {
                investig.firTri = 2;

                SuspicionScore += 60.f;
                DangerScore += 10.f;
                triSucs += 10;

                continue;
            }

            if (AK7ClothBase* Clo = Cast<AK7ClothBase>(Ev))
            {
                if (Clo->blood > 0)
                {
                    investig.firTri = 7;

                    SuspicionScore += 70.f;
                    DangerScore += 8.f;
                    triSucs += 20;

                    if (Clo->blood > 5)
                    {
                        SuspicionScore += 20.f;
                        triSucs += 10;
                    }

                    if (Clo->faction == FString("poli") ||
                        Clo->faction == FString("milt"))
                    {
                        SuspicionScore += 20.f;
                        DangerScore += 10.f;
                        triSucs += 10;
                    }
                }

                if (Clo->holesBreaks > 0)
                {
                    SuspicionScore += 8.f;

                    if (Clo->holesBreaks > 5)
                    {
                        SuspicionScore += 12.f;
                        triSucs += 5;
                    }

                    if (Clo->faction == FString("poli") ||
                        Clo->faction == FString("milt"))
                    {
                        SuspicionScore += 20.f * AgeWeight;
                    }
                }

                if (investig.firTri == 0)
                {
                    investig.firTri = 8;// possible stealing of cloths
                    SuspicionScore += 6.f;
                }

                continue;
            }
        }
        else
        {
            if (AK7WeaponsBase* Weapon = Cast<AK7WeaponsBase>(Ev))
            {
                SuspicionScore += 18.f * EvidenceWeight;
                DangerScore += 5.f * EvidenceWeight;

                if (DistFromScene < 150.f)// near the scene sreach give more detials
                {
                    SuspicionScore += 15.f * AgeWeight;
                }

                if (investig.firTri == 1 || investig.firTri == 3 || investig.firTri == 6 || investig.firTri == 7)
                {
                    curTri = 7;

                    SuspicionScore += 100.f * AgeWeight * (DistWeight * 0.5f);
                    triSucs += FMath::RoundToInt(20.f * EvidenceWeight);

                    // Body + weapon + known suspect is a strong connected theory.
                    if (investig.curSuspect)
                    {
                        curTri = 9;
                        SuspicionScore += 35.f * EvidenceWeight;
                        triSucs += 50;
                    }

                    // Weapon very far from the victim weakens the direct connection.
                    if (DistFromScene > 700.f)
                    {
                        triSucs -= 15;
                    }
                }
                else if (investig.firTri == 4)
                {
                    curTri = 9;

                    SuspicionScore += 160.f * AgeWeight * (DistWeight * 0.25f);
                    triSucs += FMath::RoundToInt(35.f * EvidenceWeight);

                    if (DistFromScene < 150.f)
                    {
                        triSucs += 20;
                    }
                    if (DistFromScene > 700.f)
                    {
                        triSucs -= 45 * AgeWeight;
                    }
                }
                else if (investig.firTri == 8)
                {
                    // Abandoned clothing followed by a weapon can mean disguise after violence.
                    curTri = 7;

                    SuspicionScore += 45.f * EvidenceWeight;
                    triSucs += 20;
                }

                continue;
            }
            else if (AK7ClothBase* Clo = Cast<AK7ClothBase>(Ev))
            {
                const bool HasBlood = Clo->blood > 0;// for not check that evry time.
                const bool IsDamaged = Clo->holesBreaks > 0;
                const bool IsAuthorityCloth = Clo->faction == FString("poli") || Clo->faction == FString("milt");

                if (investig.firTri == 7 || investig.firTri == 1 || investig.firTri == 3 || investig.firTri == 6)
                {
                    if (HasBlood)
                    {
                        curTri = 7;

                        SuspicionScore += 35.f * AgeWeight * (DistWeight * 0.5f);
                        triSucs += FMath::RoundToInt(25.f * EvidenceWeight);

                        if (Clo->blood > 50)
                        {
                            SuspicionScore += 20.f * EvidenceWeight;
                            triSucs += 15;
                        }

                        if (DistFromScene < 150.f)
                        {
                            triSucs += 15 * AgeWeight;
                        }
                        else if (DistFromScene > 700.f)
                        {
                            triSucs -= 20 * AgeWeight;
                        }
                    }

                    if (IsDamaged)
                    {
                        SuspicionScore += 12.f * EvidenceWeight;
                        triSucs += 5;
                    }
                }

                if (investig.firTri == 4)
                {
                    if (HasBlood)
                    {
                        curTri = 9;

                        SuspicionScore += 160.f * AgeWeight * (DistWeight * 0.25f);
                        triSucs += FMath::RoundToInt(45.f * EvidenceWeight);

                        if (investig.curSuspect)
                        {
                            SuspicionScore += investig.curSuspect->wirdo * 0.5f;
                        }
                    }
                    else if (!IsDamaged)
                    {
                        // Normal clothing does not support the murder theory.
                        triSucs -= 4;
                    }
                }

                if (investig.firTri == 8)
                {
                    // More abandoned clothing supports theft/disguise theory.
                    curTri = 8;
                    SuspicionScore += 15.f * EvidenceWeight;
                    triSucs += 12;

                    if (HasBlood)
                    {
                        // Clothing theft may now connect to violence.
                        curTri = 7;
                        SuspicionScore += 40.f * EvidenceWeight;
                        triSucs += 20;
                    }
                }

                if (HasBlood && IsDamaged)
                {
                    SuspicionScore += 18.f * EvidenceWeight;
                    DangerScore += 8.f * EvidenceWeight;
                    triSucs += 10;
                }

                if (IsAuthorityCloth)
                {
                    if (Clo->faction == FString("poli"))
                    {
                        HelpScore += 6.f * AgeWeight;
                    }
                    else
                    {
                        DangerScore += 4.f * AgeWeight;
                    }

                    // Abandoned bloody police/military clothes may mean impersonation.
                    if (HasBlood || IsDamaged)
                    {
                        curTri = 8;
                        SuspicionScore += 25.f * EvidenceWeight;
                        DangerScore += 4.f * AgeWeight;
                        triSucs += 15;
                    }
                }
                else
                {
                    SuspicionScore += 8.f * EvidenceWeight;
                }

                continue;
            }
            else if (AK7Npc* N = Cast<AK7Npc>(Ev))
            {
                const float Speed = N->GetVelocity().Size();
                const float DistToTrigger =
                    IsValid(investig.TriggerActor)
                    ? FVector::Dist(N->GetActorLocation(),
                        investig.TriggerActor->GetActorLocation())
                    : DistFromScene;

                const bool IsNearTrigger = DistToTrigger < 300.f;// one big hole, if he was near but only minute after the thing is wierd maye he right now come.
                const bool IsVeryNearTrigger = DistToTrigger < 120.f;
                const bool IsRunning = Speed > 400.f;
                const bool IsMovingFast = Speed > 250.f;

                if (investig.firTri == 7 || investig.firTri == 1 || investig.firTri == 3 || investig.firTri == 6)
                {
                    if (N->sol > 1)
                    {
                        float SuspectValue = N->wirdo;

                        if (IsNearTrigger)
                        {
                            SuspectValue += 20.f;
                        }

                        if (IsVeryNearTrigger)
                        {
                            SuspectValue += 15.f;
                        }

                        if (IsRunning)
                        {
                            SuspectValue += 25.f;
                        }
                        else if (IsMovingFast)
                        {
                            SuspectValue += 10.f;
                        }

                        SuspectValue += idsMem[N->ObId] / 2.f;

                        // Replace current suspect only when this person looks more suspicious. it system for not stuck on guy that you see near body in 1 second and after second see another guy witk ak an more
                        if (!investig.curSuspect)
                        {
                            investig.curSuspect = N;
                            triSucs += 10;
                        }
                        else
                        {
                            float CurrentSuspectValue = investig.curSuspect->wirdo + idsMem[investig.curSuspect->ObId] / 2.f;

                            if (SuspectValue > CurrentSuspectValue)
                            {
                                investig.curSuspect = N;
                                triSucs += 8;
                            }
                            else if (investig.curSuspect == N)
                            {
                                // Same person appears again around connected evidence.
                                triSucs += 12;
                            }
                        }
                        if(!investig.curSuspect){curTri = 7;}
                        SuspicionScore += SuspectValue * AgeWeight * DistWeight;

                        if (IsRunning && IsNearTrigger && AgeWeight > 0.2)
                        {
                            curTri = 9;
                            SuspicionScore += 45.f * EvidenceWeight;
                            DangerScore += 12.f;
                            triSucs += 25;
                        }
                        if (curTri == 0) {
                            curTri = investig.firTri;
                            SuspicionScore -= triSucs / 10.f;
                            triSucs -= 5.f;
                        }
                        continue;
                    }

                    if (N->sol <= 1)
                    {
                        HelpScore += 15.f * EvidenceWeight;
                        SuspicionScore += 30.f * EvidenceWeight;

                        if (curTri == 7) {
                            triSucs += 35;
                        }
                        else if (investig.curSuspect) {
                            if (curTri == 9) {
                                triSucs += 35;
                            }
                            else {
                                curTri = 9;
                                triSucs += 25;
                            }
                        }
                        else {
                            curTri = 7;
                            triSucs += 25;
                        }
                        

                        // Multiple victims strongly support an attack/murder theory.
                        if (investig.firTri == 1 || investig.firTri == 3)
                        {
                            SuspicionScore += 35.f * EvidenceWeight;
                            DangerScore += 15.f * EvidenceWeight;
                            triSucs += 30;
                        }

                        if (DistToTrigger < 250.f)
                        {
                            triSucs += 15;
                        }
                        else if (DistToTrigger > 800.f)
                        {
                            // Could be a separate incident.
                            triSucs -= 10;
                        }
                        continue;
                        //if (firTri == 2 || curTri == 2) { //need check maybe suicide now i cant add that becuase it break the past rule if first what we find was pistol.
                        //
                        //}
                        
                    }
                }

                if (investig.firTri == 4)
                {
                    if (investig.curSuspect == N)
                    {
                        SuspicionScore += 12.f * EvidenceWeight;
                        triSucs += 8;

                        if (IsRunning)
                        {
                            curTri = 9;
                            SuspicionScore += 30.f * EvidenceWeight;
                            triSucs += 15;
                        }
                    }
                    else if (IsValid(investig.curSuspect) && N->wirdo > investig.curSuspect->wirdo && IsNearTrigger)
                    {
                        // another person may be a more sus suspect.
                        investig.curSuspect = N;
                        SuspicionScore += 20.f * EvidenceWeight;
                        triSucs -= 5;
                    }
                }

                if (investig.firTri == 8)
                {
                    // Person Near abandoned clothes may own them, steal them, or change disguise.
                    if (IsNearTrigger)
                    {
                        investig.curSuspect = N;
                        curTri = 8;

                        SuspicionScore += 20.f * EvidenceWeight;
                        triSucs += 15;

                        if (N->wirdo > 25.f || IsRunning)
                        {
                            SuspicionScore += 25.f * EvidenceWeight;
                            triSucs += 25;
                        }
                    }
                }

                if (N->sol <= 1)
                {
                    HelpScore += 15.f * AgeWeight;
                    DangerScore += 6.f * AgeWeight;
                    SuspicionScore += 15.f * AgeWeight;
                }
                else if (N->sol < 4) // Injured, need help.
                {
                    HelpScore += 25.f * AgeWeight;
                    DangerScore += 6.f * AgeWeight;
                    SuspicionScore += 15.f * AgeWeight;
                }

                SuspicionScore += N->wirdo * AgeWeight;

                if (IsRunning)
                {
                    SuspicionScore += 12.f * AgeWeight;

                    if (IsNearTrigger)
                    {
                        SuspicionScore += 15.f * EvidenceWeight;
                    }
                }

                // calm healthy person near an injured person may be helping, not attacking.
                if (N->sol == 4 &&
                    Speed < 100.f &&
                    N->wirdo < 10.f &&
                    investig.firTri == 3)
                {
                    HelpScore += 5.f * EvidenceWeight;
                    SuspicionScore -= 4.f * EvidenceWeight;
                    triSucs -= 3;
                }

                continue;
            }
        }

    }
    investig.xue = Count;
    updTheo(triSucs, curTri);
    investig.Suspicion += SuspicionScore;
    investig.MedicalConcern += HelpScore;
    investig.Danger += DangerScore;

    // since dec is 0.5-1.5 we dp split to five ifs low dec, low normal, normal, more dec then norm and high dec
    // and how panic or chill he is 0.5-1.5 
    if (dec < 0.75) {
        if (chill < 0.75) {// parnoia man. no logic only scare.
            if (investig.Danger > 37) {
                
                CurInvesActi = EInvestigationAction::LeaveScene;
                return;
            }
            if (curTri == 7 && triSucs > 25 && triSucs < 60)
            {
                if (PrevInvesActi == EInvestigationAction::LookAround) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else {
                    CurInvesActi = EInvestigationAction::LookAround;
                }
                
                return;
            }
            
            if (curTri == 9 && triSucs > 25) {
               CurInvesActi = EInvestigationAction::FindHelp;
               return;
            }

            if (investig.MedicalConcern > 37) {
                if (curTri == 7 || curTri == 4 || curTri == 9) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                    return;
                }
                else if (investig.Suspicion > 28) {

                    if (PrevInvesActi == EInvestigationAction::LookAround) {
                        CurInvesActi = EInvestigationAction::FindHelp;
                    }
                    else {
                        CurInvesActi = EInvestigationAction::LookAround;
                    }
                    return;
                }
                if (investig.Danger > 27) {
                    CurInvesActi = EInvestigationAction::LeaveScene;
                    return;
                }
                CurInvesActi = EInvestigationAction::FindHelp;
            }
            if (curTri == 4) {
                if (PrevInvesActi == EInvestigationAction::FollowPerson) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else {
                    CurInvesActi = EInvestigationAction::FollowPerson;
                }
                return;
            }
            return;
        }
        else if (chill < 1.25) {

        }
        else {
            if (curTri == 9 && triSucs > 95) {
                CurInvesActi = EInvestigationAction::FindHelp;
                return;
            }
            if (curTri == 9 && triSucs > 65) {
                if (PrevInvesActi == EInvestigationAction::AskPerson) {
                    CurInvesActi = EInvestigationAction::FindHelp;// that also need change by the answer of the man but now it not count and not mean so like that.
                }
                else {
                    CurInvesActi = EInvestigationAction::AskPerson;
                }
                return;
            }
            if (curTri == 9 && triSucs > 25) {
                if (PrevInvesActi == EInvestigationAction::FollowPerson)
                {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else if (PrevInvesActi == EInvestigationAction::Approach)
                {
                    CurInvesActi = EInvestigationAction::FollowPerson;
                }
                else
                {
                    CurInvesActi = EInvestigationAction::Approach;
                }
                return;
            }
            if (investig.Danger > 70) {
                CurInvesActi = EInvestigationAction::FindHelp;
                return;
            }
            if (investig.MedicalConcern > 37) {
                CurInvesActi = EInvestigationAction::HelpPerson;
                return;
            }
            if (curTri == 7 && triSucs > 25) {
                CurInvesActi = EInvestigationAction::FindHelp;
                return;
            }
            if (PrevInvesActi == EInvestigationAction::Approach) {
                CurInvesActi = EInvestigationAction::LeaveScene;
            }
            else {
                CurInvesActi = EInvestigationAction::Approach;
            }
            return;
        }

    }
    else if (dec < 0.9) {
        if (chill < 0.75) {

        }
        else if (chill < 1.25) {

        }
        else {

        }
    }
    else if (dec < 1.1) {
        if (chill < 0.75) {

        }
        else if (chill < 1.25) {

        }
        else {

        }
    }
    else if (dec < 1.35) {
        if (chill < 0.75) {

        }
        else if (chill < 1.25) {

        }
        else {

        }
    }
    else {
        if (chill < 0.75) {

        }
        else if (chill < 1.25) {

        }
        else {

        }
    }
    
    CurInvesActi = EInvestigationAction::Approach;

}

void AK7Civilian::endInvestg() {

}