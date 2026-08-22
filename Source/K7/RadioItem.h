// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RadioItem.generated.h"
class ADmgDealer;
UCLASS()
class K7_API ARadioItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARadioItem();
	float getInfoById(int32 id);
	void setInfoById(int32 id, uint8 info);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ADmgDealer* acces;

};
