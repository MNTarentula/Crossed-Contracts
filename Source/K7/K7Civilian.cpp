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
#include "K7BrainNpc.h"

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

    UK7BrainNpc::randomPi(Target, GetWorld(), AICon);
    NoButI = true;
    PointB = Target;
}
// Called every frame
void AK7Civilian::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
    float Dist = FVector::Dist(GetActorLocation(), PointB);

    if (Dist < 50.f && (curAreaT == nullptr || curAreaT->TaskType == ETaskType::None))// if not had a task just not stop wander and find new thing to look at
    {
        NoButI = false;
        ft();
    }
    
    if (scary > 300 * chill) {//it scary setter and maker, if currentlly the npc is over the threshold he start run to safe place, because seacurity and safe of the person is reflex and after go the task and other thinks

        if (curAreaT == nullptr || curAreaT->TaskType != ETaskType::Safe) {
            setter(ETaskType::Safe);//setter for safe,and after excute the run to the safe place!.
            i();
        }
    }
    else if (healthC > 25.f * chill) {// very not chill or just panic man see blood run to medic (12.5), and more chill can get injured but start invastigate if what (37.5)
        if (curAreaT == nullptr || curAreaT->TaskType != ETaskType::healthCar) {
            setter(ETaskType::healthCar);
            i();
        }
    }
    else if (interst > 80 && (currekNpc || currekAct) && !investig.Active) {//it interst setter
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
    if ((curAreaT == nullptr || curAreaT->TaskType == ETaskType::None ) && !NoButI) {
        randomP(PointB);
    }
    FRotator TargetRotation =(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),PointB)- GetActorRotation()).GetNormalized();
    if (currekNpc) {
        TargetRotation = (UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), currekNpc->GetActorLocation()) - GetActorRotation()).GetNormalized();
    }
	TargetRotation.Yaw = FMath::Clamp(TargetRotation.Yaw, -MaxLookAngle, MaxLookAngle);
	TargetRotation.Pitch = FMath::Clamp(TargetRotation.Pitch, -MaxLookAngle, MaxLookAngle);
	TargetRotation.Roll = 0.0f;

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
        2.5f / active, 
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
    UE_LOG(LogTemp, Warning, TEXT("ft() called and the time is %f"), GetWorld()->GetTimeSeconds());
    int32 RandomNum = FMath::RandRange(1, 100);
    PointB = GetActorLocation();
    float max = 1000.f;
    healthC = countHealth();//count if need go to medic or not.
    AActor* currentC = whatMostInterstT(max, 180.f);
    currekNpc = Cast<AK7Npc>(currentC);
    currekAct = Cast<AK7InterstAct>(currentC);
    if (currentC) {
        if (!investig.Active ) {
            PointB = currentC->GetActorLocation();
        } 
    }
    else { // random if nothing intersting got find to watch.
        AActor* planB = getNearstNpDir(max, 180.f);
        currekNpc = Cast<AK7Npc>(planB);
        if (planB) {
            if (!investig.Active) {
                PointB = planB->GetActorLocation();
            }
        }
        else {
            FVector NormalizedDirection = GetActorForwardVector().GetSafeNormal();
            float RandomDistance = FMath::FRandRange(0.0f, max);
            float ConeHalfAngleDegrees = 15.0f;
            FVector RandomizedDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(NormalizedDirection, ConeHalfAngleDegrees);
            if (!investig.Active) {
                PointB = GetActorLocation() + (RandomizedDirection * RandomDistance);
            }
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
        if (scary > 300 * chill) {
            CurThreat = currekNpc->GetActorLocation();
        }
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
    if (investig.InterStats > 0 && investig.Active) {//it make ok not over ride invastigate
        finalTask = ETaskType::None;
    }
    else {
        /*endInvestg();*/
    }
    if (scary > 300 * chill) {
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

    if (curAreaT->TaskType == ETaskType::Safe) {// if safe we take the safe location most long away from the threat location

        float minRename = 0.f;
        FVector WP = FVector::ZeroVector;
        AATaskArea* AreaT = nullptr;
        for (AATaskArea* Area : Areas)
        {
            if (!Area)
                continue;
            if (Area->TaskType == curAreaT->TaskType)
            {
                float Distance = FVector::Distance(CurThreat, Area->location);//dis bet threat and safe it it bigger then past we go to the new one like that! 

                if (Distance > minRename)
                {
                    minRename = Distance;
                    WP = Area->location;
                    AreaT = Area;
                }
            }
        }
        if (minRename != 0.f) {
            randomP(WP);
            if (AreaT) {
                curAreaT = AreaT;
            }
        }
        else {
            randomP(CurThreat + GetActorRightVector()*1000);
        }
    }
    else if (curAreaT->TaskType == ETaskType::healthCar) {
        AK7Npc* isMedic = findHealth(5000.f, 360);
        if (IsValid(isMedic)) { // Safe check
            randomP(isMedic->GetActorLocation());
        }
    }
    else {
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
    float dis= FVector::Dist(GetActorLocation(), CurThreat);
    if (dis >= 500.f) { scary -= 4; }// only go down if threat last known location is not near enough
    intFallMen();//function to drop intrest to men only objects becuase i need that the npc can look on same npc after time 
}
void AK7Civilian::intFallMen() {
    for (int i = 0; i < idsMem.Num();i++) {
        if (manger) {
            if (manger->chFoMe(i)) {// check for men (it only check for one id(optimzation))
                idsMem[i] -= 4;
            }
        }
    }
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
            UE_LOG(LogTemp, Warning, TEXT("cur id is: %d"),curId);
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
            UE_LOG(LogTemp, Warning, TEXT("cur id is: %d"), curId);
        }
        AK7ClothBase* clo = Cast<AK7ClothBase>(Actor);
        if (clo) {
            curId = clo->ObId;
            UE_LOG(LogTemp, Warning, TEXT("cur id is: %d"), curId);
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
        UE_LOG(LogTemp, Warning, TEXT("cur interst is: %d"), currentI);
        if (idsMem[curId] <= currentI ) {
            idsMem[curId] = currentI;
            if (currentI > maximums) {
                maximums = currentI;
                mostHave = Actor;
            }
        }
        
    }
    return mostHave;
}
ACharacter* AK7Civilian::getNearstNpDir(float MaxRange, float MaxAngleDegrees) {return UK7BrainNpc::getNearstNpDir(MaxRange, MaxAngleDegrees, GetWorld(), this);}
//************************************************ health care system    ***************************************************************//

float AK7Civilian::countHealth() {
    float ret=maxHp - curHP;
    if (sol <= 1) {
        ret = 0;
    }
    if (scary < 150 * chill) {
        ret *= 1.5;
    }

    return ret;
}

AK7Npc* AK7Civilian::findHealth(float MaxRange, float MaxAngleDegrees) {
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this); // that the npc not watch at him self 

    TArray<AActor*> OverlappingActors;
    float NearestDistanceSquared = MAX_flt;
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

    AK7Npc* NearestNPC = nullptr;
    

    // Get your forward vector and current location
    FVector ForwardDir = GetActorForwardVector();
    FVector CurrentLoc = GetActorLocation();

    // 3. Filter by Direction and Find Nearest character
    for (AActor* Actor : OverlappingActors)
    {
        AK7Npc* NPC = Cast<AK7Npc>(Actor);
        if (NPC)
        {
            
            if (NPC->medic) {
                FVector DirToNPC = NPC->GetActorLocation() - CurrentLoc;
                float DistanceSquared = DirToNPC.SizeSquared();
                if (DistanceSquared < NearestDistanceSquared) { // select most nearest medic that can help (health care base)
                    NearestDistanceSquared = DistanceSquared;
                    NearestNPC = NPC;
                }
            }
            
        }
    }

    return NearestNPC;
}
//************************************************ find help of ppls     ***************************************************************//

AK7Npc* AK7Civilian::findHalp(float MaxRange, float MaxAngleDegrees) {
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    TArray<AActor*> IgnoredActors;
    IgnoredActors.Add(this); 
    TArray<AActor*> OverlappingActors;
    float NearestDistanceSquared = MAX_flt;
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        GetActorLocation(),
        MaxRange,
        ObjectTypes,
        ACharacter::StaticClass(),
        IgnoredActors,
        OverlappingActors
    );

    AK7Npc* NearestNPC = nullptr;
    FVector CurrentLoc = GetActorLocation();

    for (AActor* Actor : OverlappingActors)
    {
        AK7Npc* NPC = Cast<AK7Npc>(Actor);
        if (NPC)
        {

            if (NPC->cloth) {
                if (cloth->faction == FString("poli") || cloth->faction == FString("mili")) {
                    FVector DirToNPC = NPC->GetActorLocation() - CurrentLoc;
                    float DistanceSquared = DirToNPC.SizeSquared();
                    if (DistanceSquared < NearestDistanceSquared) { 
                        NearestDistanceSquared = DistanceSquared;
                        NearestNPC = NPC;
                    }
                } 
            }
        }
    }

    return NearestNPC;
}

//************************************************ invastigate functions ***************************************************************//

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
    UK7BrainNpc::updTheo(sucs, tri, investig);
}

void AK7Civilian::decInvestAc() {
    GetWorldTimerManager().ClearTimer(excuTim);
    if (!investig.Active) { return; }// safe check for not call it when we leave and excuter cal it by  the timers like that, just check safe.
    const int32 Count = investig.evidAct.Num();
    if (Count == 0)
    {
        CurInvesActi = EInvestigationAction::LookAround;
        return;
    }

    UK7BrainNpc::teamBuilding(GetWorld(), investig, idsMem, trust);

    // since dec is 0.5-1.5 we dp split to five ifs low dec, low normal, normal, more dec then norm and high dec
    // and how panic or chill he is 0.5-1.5 
    if (dec < 0.75) {
        if (chill < 0.75) {// parnoia man. no logic only scare.
            if (investig.Danger > 37) {
                
                CurInvesActi = EInvestigationAction::LeaveScene;
                excuter();
                return;
            }
            if (investig.curTri == 7 && investig.triSucs > 25 && investig.triSucs < 60)
            {
                if (PrevInvesActi == EInvestigationAction::LookAround) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else {
                    CurInvesActi = EInvestigationAction::LookAround;
                }
                excuter();
                return;
            }
            
            if (investig.curTri == 9 && investig.triSucs > 25) {
               CurInvesActi = EInvestigationAction::FindHelp;
               excuter();
               return;
            }

            if (investig.MedicalConcern > 37) {
                if (investig.curTri == 7 || investig.curTri == 4 || investig.curTri == 9) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                    excuter();
                    return;
                }
                else if (investig.Suspicion > 28) {

                    if (PrevInvesActi == EInvestigationAction::LookAround) {
                        CurInvesActi = EInvestigationAction::FindHelp;
                    }
                    else {
                        CurInvesActi = EInvestigationAction::LookAround;
                    }
                    excuter();
                    return;
                }
                if (investig.Danger > 27) {
                    CurInvesActi = EInvestigationAction::LeaveScene;
                    excuter();
                    return;
                }
                CurInvesActi = EInvestigationAction::FindHelp;
            }
            if (investig.curTri == 4) {
                if (PrevInvesActi == EInvestigationAction::FollowPerson) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else {
                    CurInvesActi = EInvestigationAction::FollowPerson;
                }
                excuter();
                return;
            }
            CurInvesActi = EInvestigationAction::Approach;
            excuter();
            return;
        }
        else if (chill < 1.25) {// normal man not super chill and not parnoied
            if (investig.Danger > 70) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }
            if (investig.curTri == 9 && investig.triSucs > 75) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }
            if (investig.curTri == 9 && investig.triSucs > 35) {
                if (PrevInvesActi == EInvestigationAction::AskPerson) {
                    CurInvesActi = EInvestigationAction::FindHelp;// that also need change by the answer of the man but now it not count and not mean so like that.in idea it change the triSucs so no worries
                }
                else {
                    CurInvesActi = EInvestigationAction::AskPerson;
                }
                excuter();
                return;
            }
            if (investig.MedicalConcern > 37) {
                if (PrevInvesActi == EInvestigationAction::LookAround) {
                    CurInvesActi = EInvestigationAction::HelpPerson;
                }
                else {
                    CurInvesActi = EInvestigationAction::LookAround;
                }
                excuter();
                return;
            }
            if (investig.curTri == 9 && investig.triSucs > 15) {
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
                excuter();
                return;
            }
            if (investig.curTri == 7 && investig.triSucs > 25) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }
            
            if (PrevInvesActi == EInvestigationAction::LookAround)
            {
                CurInvesActi = EInvestigationAction::LeaveScene;
            }
            else if (PrevInvesActi == EInvestigationAction::Approach)
            {
                CurInvesActi = EInvestigationAction::LookAround;
            }
            else
            {
                CurInvesActi = EInvestigationAction::Approach;
            }
            excuter();
            return;
        }
        else {
            if (investig.curTri == 9 && investig.triSucs > 95) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }
            if (investig.curTri == 9 && investig.triSucs > 65) {
                if (PrevInvesActi == EInvestigationAction::AskPerson) {
                    CurInvesActi = EInvestigationAction::FindHelp;// that also need change by the answer of the man but now it not count and not mean so like that.
                }
                else {
                    CurInvesActi = EInvestigationAction::AskPerson;
                }
                excuter();
                return;
            }
            if (investig.curTri == 9 && investig.triSucs > 25) {
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
                excuter();
                return;
            }
            if (investig.Danger > 70) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }
            if (investig.MedicalConcern > 37) {
                CurInvesActi = EInvestigationAction::HelpPerson;
                excuter();
                return;
            }
            if (investig.curTri == 7 && investig.triSucs > 25) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }
            if (PrevInvesActi == EInvestigationAction::Approach) {
                CurInvesActi = EInvestigationAction::LeaveScene;
            }
            else {
                CurInvesActi = EInvestigationAction::Approach;
            }
            excuter();
            return;
        }

    }
    else if (dec < 0.9) {
        if (chill < 0.75) {
            if (investig.Danger > 37) {
                if (PrevInvesActi == EInvestigationAction::KeepDistance) {
                    CurInvesActi = EInvestigationAction::LeaveScene;
                }
                else {
                    CurInvesActi = EInvestigationAction::KeepDistance;
                }
                excuter();
                return;
            }
            if (investig.curTri == 7 && investig.triSucs > 1 && investig.triSucs < 50)
            {
                if (PrevInvesActi == EInvestigationAction::LookAround) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else {
                    CurInvesActi = EInvestigationAction::LookAround;
                }
                excuter();
                return;
            }

            if (investig.curTri == 9 && investig.triSucs > 35) {
                CurInvesActi = EInvestigationAction::FindHelp;
                excuter();
                return;
            }

            if (investig.MedicalConcern > 50) {
                if (investig.curTri == 7 || investig.curTri == 4 || investig.curTri == 9) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                    excuter();
                    return;
                }
                else if (investig.Suspicion > 28) {

                    if (PrevInvesActi == EInvestigationAction::LookAround) {
                        CurInvesActi = EInvestigationAction::KeepDistance;
                    }
                    else {
                        CurInvesActi = EInvestigationAction::LookAround;
                    }
                    excuter();
                    return;
                }
                if (investig.Danger > 27) {
                    CurInvesActi = EInvestigationAction::LeaveScene;
                    excuter();
                    return;
                }
                CurInvesActi = EInvestigationAction::FindHelp;
            }
            if (investig.curTri == 4) {
                if (PrevInvesActi == EInvestigationAction::FollowPerson) {
                    CurInvesActi = EInvestigationAction::FindHelp;
                }
                else {
                    CurInvesActi = EInvestigationAction::FollowPerson;
                }
                excuter();
                return;
            }
            CurInvesActi = EInvestigationAction::KeepDistance;
            excuter();
            return;
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
    excuter();
}

void AK7Civilian::excuter() {
    GetWorldTimerManager().ClearTimer(invTim);
    PrevInvesActi = CurInvesActi;
    AK7Npc* xa;
    switch (CurInvesActi)
    {
    case EInvestigationAction::LookAround:

        GetWorldTimerManager().SetTimer(
            invTim,
            this,
            &AK7Civilian::ft,
            0.2f, 
            true
        );
        break;

    case EInvestigationAction::FollowPerson:
        PointB = investig.curSuspect->GetActorLocation(); // o nopt check the curssupect if nulll becuase before we call excuter that for set the current to floow need cur suspect not be null
        randomP(PointB); // for now it only follow like that but idk, 
        break;

    case EInvestigationAction::FindHelp:
        xa = findHalp(5000.f, 360);// work for find gaurd or atleast medic, and check it not suspect becuase ask help caugth suspect the suspect it self is wierd,
        if (IsValid(xa)) {
            if (xa != investig.curSuspect) {
                randomP(xa->GetActorLocation());
            }
        }
        else {
            xa = findHealth(5000.f, 360);
            if (IsValid(xa)) {
                if (xa != investig.curSuspect) {
                    randomP(xa->GetActorLocation());
                }
            }
        }

        break;

    case EInvestigationAction::LeaveScene:
        endInvestg();
        break;

    case EInvestigationAction::HelpPerson:
        PointB = investig.SceneCenter;
        randomP(PointB);
        break;

    case EInvestigationAction::Approach:
        PointB = investig.SceneCenter;
        randomP(PointB - GetActorRightVector() * 50);
        break;
    case EInvestigationAction::KeepDistance:
        randomP(investig.SceneCenter - GetActorRightVector() * 2000);
        break;
    case EInvestigationAction::AskPerson:
        //here be ask person function but now clear becuase not had that mehanic
        PointB = investig.SceneCenter;
        randomP(PointB - GetActorRightVector() * 200);
        break;
    case EInvestigationAction::None:
        //not should be possible btw
        break;
    }
    GetWorldTimerManager().SetTimer(// it the excution time if we not get dec before we will call dec(in idea it should be in each case self timer for excution for follow much bigger then 5 seconds, and for leave scene even not need that timer.
        excuTim,
        this,
        &AK7Civilian::decInvestAc,
        5.f,
        false
    );
}
void AK7Civilian::endInvestg() {
    if (!investig.Active) {
        return;
    }
    investig.Active = false;
    investig.TriggerActor = nullptr;
    investig.timeEvidFind.Empty();
    investig.evidAct.Empty();
    investig.InterStats = 0;
    investig.curSuspect = nullptr;
    if(investig.curTri != 0){ investig.pastTri = investig.curTri; } else { investig.pastTri = investig.firTri; }
    investig.curTri = 0;investig.firTri = 0;
    // fvector location (scene center) take same amount as they full or not so set it to zero not had any reason so yeah like that.(the time is same (the first time found like that)
}