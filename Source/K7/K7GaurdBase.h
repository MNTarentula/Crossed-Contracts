// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K7CombatBase.h"
#include "K7GaurdBase.generated.h"


class ARadioItem;
UCLASS()
class K7_API AK7GaurdBase : public AK7CombatBase
{
	GENERATED_BODY()
	

public:
	AK7GaurdBase();
	void settInfoById(uint8 id,float info);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio")
	TSubclassOf<ARadioItem> startRadio;
protected:
	virtual void BeginPlay() override;

};
