// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7Npc.h"
#include "ATaskArea.h"
#include "K7Civilian.generated.h"

class AATaskArea;
class AAIController;
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
	FVector PointB;
	int workspace;
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
