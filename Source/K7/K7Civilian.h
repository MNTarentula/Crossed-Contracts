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
	int workspace;
	//invastigation fundation
	FInvestigationContext investig;
	EInvestigationAction CurInvesActi;
	void strInvestg();
	void updInvestg(); 
	void decInvestAc();
	void endInvestg();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FTimerHandle tim;
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
