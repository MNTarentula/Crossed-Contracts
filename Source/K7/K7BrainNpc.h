// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "K7BrainNpc.generated.h"
class AAIController;
/**
 * 
 */
UCLASS()
class K7_API UK7BrainNpc : public UBlueprintFunctionLibrary // class, that just do split take function from civilian and split it to ai that every future ai will can use it.
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Analytics")
	static void randomPi(const FVector& Target, UWorld* worldBlock, AAIController* aicon);
};
