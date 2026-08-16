// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7GaurdBase.h"
#include "K7GaurdNpc.generated.h"

class AAsultAvtomatK;
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
};
