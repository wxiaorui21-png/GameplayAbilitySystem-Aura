// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetSpellDescription(int32 Level) override;
	virtual FString GetSpellNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();
protected:
	float GetCoolDown(float InLevel = 1.f) const;
	float GetManaCost(float InLevel = 1.f) const;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumFires = 12;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;
};
