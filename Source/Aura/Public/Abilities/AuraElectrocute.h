// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AuraBeamSpell.h"
#include "AuraElectrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()
public:
	virtual FString GetSpellDescription(int32 Level) override;
	virtual FString GetSpellNextLevelDescription(int32 Level) override;
protected:
	
	float GetCoolDown(float InLevel = 1.f) const;
	float GetManaCost(float InLevel = 1.f) const;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumBeam = 5;
};
