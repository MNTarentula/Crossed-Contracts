// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7RangedWeapons.h"
#include "AsultAvtomatK.generated.h"

UCLASS()
class K7_API AAsultAvtomatK : public AK7RangedWeapons
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsultAvtomatK();
	virtual AK7WeaponsBase* taked() override;
	virtual void Physic(bool s) override;
protected:
	// Called when the game starts or when spawned
	

	virtual void BeginPlay() override;


};
