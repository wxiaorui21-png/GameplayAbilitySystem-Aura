// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FDamageEffectParams;
/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromDefaultClass(
		AActor* TargetActor,
		FVector RadialDamageOrigin = FVector::ZeroVector,
		bool bOverrideKnockForce = false,
		FVector OverrideKnockForce = FVector::ZeroVector,
		bool bOverrideDeathForce = false,
		FVector OverrideDeathForce = FVector::ZeroVector,
		bool bOverridePitch = false,
		float OverridePitch = 0) const;
	
	UFUNCTION(BlueprintPure)
	float GetDamage() const;
	
	
protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffChance = 20.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffDamage = 5.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffDuration = 5.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DebuffFrequency = 1.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float DeathImpulseMagniture = 60.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float KnockImpulseMagniture = 60.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	float KnockChance = 60.f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Damage")
	bool bIsRadialDamage = false;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Damage")
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Damage")
	float RadialDamageOuterRadius = 0.f;
	
	
	
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage> TaggedMontages)const;
};
