// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7ClothBase.h"
#include "K7clothCivilian.generated.h"

UCLASS()
class K7_API AK7clothCivilian : public AK7ClothBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AK7clothCivilian();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
