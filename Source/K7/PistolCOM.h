#pragma once

#include "CoreMinimal.h"
#include "K7RangedWeapons.h"
#include "PistolCOM.generated.h"

UCLASS()
class K7_API APistolCOM : public AK7RangedWeapons
{
	GENERATED_BODY()

	
public:
	APistolCOM();
	virtual AK7WeaponsBase* taked() override;
	virtual void Physic(bool s) override;
protected:

	
	
	virtual void BeginPlay() override;

};