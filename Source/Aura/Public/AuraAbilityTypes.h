#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	
	FDamageEffectParams(){}
	
	UPROPERTY()
	TObjectPtr<UObject> WorldContentObject;
	
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;
	
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();
	
	UPROPERTY()
	float BaseDamage = 0.0f;
	
	UPROPERTY()
	float AbilityLevel = 0;
	
	UPROPERTY()
	float DebuffChance = 0.f;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	UPROPERTY()
	float DeathImpulseMagniture = 0.f;
	
	UPROPERTY()
	float KnockImpulseMagniture = 0.f;
	
	UPROPERTY()
	float KnockChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector ImpulseDirection = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	FVector KnockImpulseDirection = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;
	
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
};
USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	bool IsCriticalHit() const{ return bIsCriticalHit; }
	bool IsBlockHit() const{ return bIsBlockHit; }
	bool IsSuccessfulDebuff() const{ return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const{ return DebuffDamage; }
	float GetDebuffFrequency() const{ return DebuffFrequency; }
	float GetDebuffDuration() const{ return DebuffDuration; }
	TSharedPtr<FGameplayTag> GetDamageType() const{ return DamageType; }
	FVector GetImpulseDirection() const{ return ImpulseDirection; }
	FVector GetKnockImpulseDirection() const{ return KnockImpulseDirection; }
	bool GetIsRadialDamage() const { return bIsRadialDamage; }
	float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius;}
	float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius;}
	FVector GetRadialDamageOrigin() const { return RadialDamageOrigin;}
	
	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockHit(bool bInIsBlockHit) { bIsBlockHit = bInIsBlockHit; }
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) { bIsSuccessfulDebuff = bInIsSuccessfulDebuff; }
	void SetDebuffDamage(float InDebuffDamage) { DebuffDamage = InDebuffDamage; }
	void SetDebuffDuration(float InDebuffDuration) { DebuffDuration = InDebuffDuration; }
	void SetDebuffFrequency(float InDebuffFrequency) { DebuffFrequency = InDebuffFrequency; }
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	void SetImpulseDirection(const FVector& InImpulseDirection) { ImpulseDirection = InImpulseDirection; }
	void SetKnockImpulseDirection(const FVector& InKnockImpulseDirection) { KnockImpulseDirection = InKnockImpulseDirection; }
	void SetIsRadialDamage(bool bInIsRadialDamage) { bIsRadialDamage = bInIsRadialDamage; }
	void SetRadialDamageInnerRadius(float InRadialDamageInnerRadius) { RadialDamageInnerRadius = InRadialDamageInnerRadius; }
	void SetRadialDamageOuterRadius(float InRadialDamageOuterRadius) { RadialDamageOuterRadius = InRadialDamageOuterRadius; }
	void SetRadialDamageOrigin(const FVector& InRadialDamageRadius) { RadialDamageOrigin = InRadialDamageRadius; }
	/** Returns the actual struct used for serialization, subclasses must override this! */
	//复制过来时一定要修改类型
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	//复制过来时一定要修改类型
	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
protected:
	
	UPROPERTY()
	bool bIsBlockHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
	
	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	TSharedPtr<FGameplayTag> DamageType;
	
	UPROPERTY()
	FVector ImpulseDirection = FVector::ZeroVector;
	
	UPROPERTY()
	FVector KnockImpulseDirection = FVector::ZeroVector;
	
	UPROPERTY()
	bool bIsRadialDamage = false;
	
	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;
	
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};