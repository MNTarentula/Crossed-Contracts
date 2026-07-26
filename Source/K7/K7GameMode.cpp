// Copyright Epic Games, Inc. All Rights Reserved.

#include "K7GameMode.h"
#include "K7Character.h"
#include "UObject/ConstructorHelpers.h"

AK7GameMode::AK7GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
