// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "K7brainMem.generated.h"

/**
 * 
 */
UCLASS()
class K7_API UK7brainMem : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UK7brainMem();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<uint8> idsMem;
};
