// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
public:
	virtual FString GetSpellDescription(int32 Level) override;
	virtual FString GetSpellNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable,Category = "Projectile")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag,AActor* HomingTarget,bool bOverridePitch = false,float PitchOverride = 0.f);
protected:
	
	float GetCoolDown(float InLevel = 1.f) const;
	float GetManaCost(float InLevel = 1.f) const;
	
	UPROPERTY(EditDefaultsOnly)
	float ProjectileSpread = 90.f;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxNumProjectile = 5;
	
	UPROPERTY(EditDefaultsOnly)
	float HomingSpeedMin = 1600;
	
	UPROPERTY(EditDefaultsOnly)
	float HomingSpeedMax = 3600;
	
	UPROPERTY(EditDefaultsOnly)
	bool bIsHoming = true;
};
