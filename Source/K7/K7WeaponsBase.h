#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "K7WeaponsBase.generated.h"

UCLASS(Abstract) // Abstract means you can't accidentally spawn a naked base weapon
class K7_API AK7WeaponsBase : public AActor
{
	GENERATED_BODY()



public:
	AK7WeaponsBase();

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	USkeletalMeshComponent* MeshComponent;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Use(); // virtual allows Ranged and Melee to override this
	virtual AK7WeaponsBase* taked();
};