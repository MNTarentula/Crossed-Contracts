// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7GaurdBase.h"
#include "K7GaurdNpc.generated.h"

class AAsultAvtomatK;
USTRUCT()
struct FInvestigationContextG
{
	GENERATED_BODY()
	bool Active = false; // the whole struct is for gaurd (will had patrols zones, more specifics things that gaurds need 
	float InterStats = 0.f;
	FVector SceneCenter;
	AActor* TriggerActor = nullptr;
	AK7Npc* curSuspect = nullptr;
	float timeOfInvest = 0.f;
	TArray<AActor*> evidAct;
	TArray<float> timeEvidFind;
	TArray<FVector> locationsFi;
	float Suspicion = 0.f;
	float MedicalConcern = 0.f;
	float Danger = 0.f;

	int32 firTri = 0;// 1 it be dead or no sense,2 pistol on floor,3 injured or crawling, 4 suspicons man, 5 strange sound, 6 draged man,7 possible murder, 8 possible stealing of cloth, 9 the suspect is murder!
	int32 curTri = 0; // same as fir but current and not started teahory randlor intagrated
	int32 pastTri = 0;
	int32 triSucs = 0;// 0-100 it how he sure he right if he be sure more then 90 changes by dec and chill,also it can over the limit of 100 becuase confidense can over 100 in idea you can sure you right on 1000 procents
	int32 xue = 0;

};

UCLASS()
class K7_API AK7GaurdNpc : public AK7GaurdBase
{
	GENERATED_BODY()
public:
	AK7GaurdNpc();
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AAsultAvtomatK> weap;
	void shotAtTarget(AK7Npc* tar);
protected:
	FInvestigationContextG investigG;
};
