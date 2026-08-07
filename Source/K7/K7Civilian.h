// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7Npc.h"
#include "ATaskArea.h"
#include "K7Civilian.generated.h"

class AATaskArea;
class AAIController;
enum class EInvestigationAction : uint8
{
	None,
	LookAround,
	Approach,
	AskPerson,
	HelpPerson,
	FindHelp,
	KeepDistance,
	FollowPerson,
	LeaveScene
};
struct FInvestigationContext
{
	bool Active = false;
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
	int32 triSucs = 0;// 0-100 it how he sure he right if he be sure more then 90 changes by dec and chill
	int32 xue = 0;

	bool SawBody = false;
	bool SawWeapon = false;
	bool HeardHelp = false;
	bool SawRunningPerson = false;
};

UCLASS()
class K7_API AK7Civilian : public AK7Npc
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AK7Civilian();
	UPROPERTY()
	AAIController* AICon = nullptr;
	TArray<AATaskArea*> Areas;
	AATaskArea* curAreaT;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	ACharacter* getNearstNpDir(float MaxRange, float MaxAngleDegrees);
	AActor* whatMostInterstT(float MaxRange, float MaxAngleDegrees);
	FVector PointB;
	bool NoButI = false;
	int workspace;
	//invastigation fundation
	FInvestigationContext investig;
	EInvestigationAction CurInvesActi = EInvestigationAction::None;
	EInvestigationAction PrevInvesActi = EInvestigationAction::None;
	void strInvestg();
	void updInvestg(); 
	void updTheo(int32 triSucs,int32 curTri);
	void excuter();
	void decInvestAc();
	void endInvestg();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadOnly, Category = "ft")
	FTimerHandle tim;
	UPROPERTY(BlueprintReadOnly, Category = "invest ft")
	FTimerHandle invTim;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FTimerHandle zapoi;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FTimerHandle tt;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FTimerHandle ers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<uint8> idsMem;

	void time();
	void eers();
	void Zapoier();
	void needTick();
	void ft();
	void ok();
	void i();
	void setter(ETaskType a);
	void ctf();
	void randomP(const FVector& Target);
};
