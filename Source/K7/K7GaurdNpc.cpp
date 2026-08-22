// Fill out your copyright notice in the Description page of Project Settings.


#include "K7GaurdNpc.h"
#include "AsultAvtomatK.h"
#include "K7BrainNpc.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "K7CombatBase.h"
#include "K7ClothBase.h"
#include "K7WeaponsBase.h"
#include "DmgDealer.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "ATaskArea.h"
#include <iostream>
#include <algorithm>
#include "K7InterstAct.h"

AK7GaurdNpc::AK7GaurdNpc() {
	//now the gaurd not reeady, i just decide to do patrol first, after fight,invastigate, and after health, help other people,  and oop in the end.
}

void AK7GaurdNpc::BeginPlay() {
	Super::BeginPlay();
	//spawn ak47 to the gaurd. becasue  i test after can be any other main weapon. for now one ak47 spawn and socket to the hand.
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); // Spawns 100 units in front
	FRotator SpawnRotation = GetActorRotation();


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; 
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // Handles collisions safely

	CollectWeapon(GetWorld()->SpawnActor<AAsultAvtomatK>(
		weap,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	));
	socketstuck(CurrentWeapon, nullptr);

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

void AK7GaurdNpc::randomP(const FVector& Target){if (!AICon){ return; }UK7BrainNpc::randomPi(Target, GetWorld(), AICon);NoButI = true;PointB = Target;}

void AK7GaurdNpc::shotAtTarget(AK7Npc* tar) {
	if (IsValid(tar)) {
		if (CurrentWeapon) {
			FVector Direction = tar->GetActorLocation() - GetActorLocation();
			Fire(GetActorLocation(), Direction);
		}
		else {
			SwitchWeapon(0);
			if (CurrentWeapon) {
				FVector Direction = tar->GetActorLocation() - GetActorLocation();
				Fire(GetActorLocation(), Direction);
			}
			return;

		}
	}
	else {
		tar = Cast<AK7Npc>(UK7BrainNpc::getNearstNpDir(5000.f, 180.f, GetWorld(), this));
		if (CurrentWeapon) {
			FVector Direction = tar->GetActorLocation() - GetActorLocation();
			Fire(GetActorLocation(), Direction);
		}
		else {
			SwitchWeapon(0);
			if (CurrentWeapon) {
				FVector Direction = tar->GetActorLocation() - GetActorLocation();
				Fire(GetActorLocation(), Direction);
			}
			return;

		}
	}return;
}
void AK7GaurdNpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AK7GaurdNpc::ft() {// first think it faster then ok happend and it just anlyze current world around decicde what to look and the emrgnsy fealling not toilet not work, just if scary,ivistigate or take health care.
    currekNpc = nullptr;
    currekAct = nullptr;
    UE_LOG(LogTemp, Warning, TEXT("ft() called and the time is %f"), GetWorld()->GetTimeSeconds());
    int32 RandomNum = FMath::RandRange(1, 100);
    PointB = GetActorLocation();
    float max = 1000.f;
    AActor* currentC = whatMostInterstT(max, 180.f);
    currekNpc = Cast<AK7Npc>(currentC);
    currekAct = Cast<AK7InterstAct>(currentC);
    if (currentC) {
        if (!investig.Active) {
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
        int Timeinters = idsMem[currekNpc->ObId] + currekNpc->wirdo / 2;

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
            int curId = 0;
            AK7WeaponsBase* allreadyHappend = Cast<AK7WeaponsBase>(currentC);
            if (allreadyHappend) {
                curId = allreadyHappend->ObId;
            }
            AK7ClothBase* clo = Cast<AK7ClothBase>(currentC);
            if (clo) {
                curId = clo->ObId;
            }
            interst = idsMem[curId];
        }

    }
}
void AK7GaurdNpc::setter(ETaskType a) {// just a setter of current type of task, not to where go, it set what to do by the input.
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
void AK7GaurdNpc::ok() { // it function of hard thinking that decide by current needs and prority of them body what task currentlly more importnat.


    //int biggestNeed = -1;
    //int secondNeed = -1;
    int toiletScore;
    int hungScore;
    int postScore;

    postScore = work * (workP + FMath::Clamp(toilet / 100, 0, 5)) * 2 / chill; // if chill the work not so importnat (cuz he gaurd like that)
    toiletScore = toilet * (toiletP + FMath::Clamp(toilet / 100, 0, 5));
    hungScore = hung * (hungP + FMath::Clamp(hung / 100, 0, 5));
    
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

    if (postScore > biggestScore)
    {
        biggestScore = postScore;
        finalTask = ETaskType::post;
    }
    if (IsValid(manger) && manger->currentHelp != FVector(9999.f)) {//it make ok not over ride invastigate
        finalTask = ETaskType::Work;
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
void AK7GaurdNpc::i() { // i it excution of the body it chosse nearest point of the task and go to them, that function not decide it do.
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
            randomP(CurThreat + GetActorRightVector() * 1000);
        }
    }
    else if (curAreaT->TaskType == ETaskType::Work) {
        if (IsValid(manger)) { randomP(manger->currentHelp); }
    }
    else if(curAreaT->TaskType == ETaskType::post){
        FVector WP = FVector::ZeroVector;
        AATaskArea* AreaT = nullptr;
        
        
        for (AATaskArea* Area : Areas)
        {
            if (!Area)
                continue;
            if (Area->TaskType == curAreaT->TaskType)
            {
                if (Area->isTaked == ObId) {
                    if (postLast == Area) {
                        continue;
                    }
                    WP = Area->location;
                    AreaT = Area;
                    postLast = Area;
                }
                

            }
        }
        if(AreaT){ randomP(WP); curAreaT = AreaT;}
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
void AK7GaurdNpc::ctf() {// called evry 2 seconds, but only where in zone of task and had that task cant be in toilet and do the task of food. in tick main logic of check that.
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
void AK7GaurdNpc::needTick() { // it update the needs evry 20 seceonds by timer zapoier or like that
    toilet += 4;
    hung += 4;
    float dis = FVector::Dist(GetActorLocation(), CurThreat);
    if (dis >= 500.f) { scary -= 4; }// only go down if threat last known location is not near enough
    intFallMen();//function to drop intrest to men only objects becuase i need that the npc can look on same npc after time 
}
void AK7GaurdNpc::intFallMen() {
    for (int i = 0; i < idsMem.Num();i++) {
        if (manger) {
            if (manger->chFoMe(i)) {// check for men (it only check for one id(optimzation))
                idsMem[i] -= 4;
            }
        }
    }
}

// first think,
AActor* AK7GaurdNpc::whatMostInterstT(float MaxRange, float MaxAngleDegrees) {
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

    int32 maximums = -1;

    for (AActor* Actor : taggedOnes)
    {
        int32 curId = 0;
        int32 currentI = -1;
        AK7Npc* NPC = Cast<AK7Npc>(Actor);
        if (NPC) {
            curId = NPC->ObId;
            UE_LOG(LogTemp, Warning, TEXT("cur id is: %d"), curId);
            currentI = NPC->interesting + NPC->GetVelocity().Size() / 20 + NPC->wirdo;//based on base interst,on velocity if the man around you fly he may take your attetion, and how sus he look (wirdo) also need count how naer you is it.
            if (NPC->cloth) {
                AK7ClothBase* fast = NPC->cloth;
                if (fast->faction == FString("poli")) {
                    currentI += 20;
                }
                else if (fast->faction == FString("milt")) {//not set now not imporntat
                    currentI += 35;
                }
                currentI += fast->blood * 5;
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
        AK7WeaponsBase* allreadyHappend = Cast<AK7WeaponsBase>(Actor);
        if (allreadyHappend) {
            curId = allreadyHappend->ObId;
            currentI = allreadyHappend->interesting / chill;
            UE_LOG(LogTemp, Warning, TEXT("cur id is: %d"), curId);
        }
        AK7ClothBase* clo = Cast<AK7ClothBase>(Actor);
        if (clo) {
            curId = clo->ObId;
            UE_LOG(LogTemp, Warning, TEXT("cur id is: %d"), curId);
            currentI = clo->interesting;
            if (clo->faction == FString("poli")) {
                currentI += 50;
            }
            else  if (clo->faction == FString("milt")) {
                currentI += 60;
            }
            currentI += clo->blood * 7;
            currentI += clo->holesBreaks * 3.5;
        }
        float dis = FVector::Distance(GetActorLocation(), Actor->GetActorLocation());
        currentI += (MaxRange - dis) / 5;
        UE_LOG(LogTemp, Warning, TEXT("cur interst is: %d"), currentI);
        if (idsMem[curId] <= currentI) {
            idsMem[curId] = currentI;
            if (currentI > maximums) {
                maximums = currentI;
                mostHave = Actor;
            }
        }

    }
    return mostHave;
}
ACharacter* AK7GaurdNpc::getNearstNpDir(float MaxRange, float MaxAngleDegrees) { return UK7BrainNpc::getNearstNpDir(MaxRange, MaxAngleDegrees, GetWorld(), this); }

//timers
void AK7GaurdNpc::eers() {// timer setter for the ok()

    GetWorldTimerManager().ClearTimer(ers);
    GetWorldTimerManager().SetTimer(
        ers,
        this,
        &AK7GaurdNpc::ok,
        20.f / active,
        true
    );
}
void AK7GaurdNpc::time() {// timer setter for the ft()

    GetWorldTimerManager().ClearTimer(tim);
    GetWorldTimerManager().SetTimer(
        tim,
        this,
        &AK7GaurdNpc::ft,
        2.5f / active,
        true
    );
}
void AK7GaurdNpc::Zapoier() {// timer setter for the needs.

    GetWorldTimerManager().ClearTimer(zapoi);
    GetWorldTimerManager().SetTimer(
        zapoi,
        this,
        &AK7GaurdNpc::needTick,
        20.f,
        true
    );
}