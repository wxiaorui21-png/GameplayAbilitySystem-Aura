// Mr.Wang


#include "Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass,1);
	const float DamageMagnitude = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle,DamageType,DamageMagnitude);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(),UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromDefaultClass(AActor* TargetActor,
	FVector RadialDamageOrigin, bool bOverrideKnockForce, FVector OverrideKnockForce, bool bOverrideDeathForce,
	FVector OverrideDeathForce, bool bOverridePitch, float OverridePitch) const
{
	FDamageEffectParams DamageEffectParams;
	DamageEffectParams.WorldContentObject = GetAvatarActorFromActorInfo();
	DamageEffectParams.DamageGameplayEffectClass = DamageEffectClass;
	DamageEffectParams.DamageType = DamageType;
	DamageEffectParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	DamageEffectParams.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	DamageEffectParams.DebuffChance = DebuffChance;
	DamageEffectParams.DebuffDuration = DebuffDuration;
	DamageEffectParams.DebuffFrequency = DebuffFrequency;
	DamageEffectParams.DebuffDamage = DebuffDamage;
	DamageEffectParams.DeathImpulseMagniture = DeathImpulseMagniture;
	DamageEffectParams.KnockImpulseMagniture = KnockImpulseMagniture;
	DamageEffectParams.KnockChance = KnockChance;
	DamageEffectParams.bIsRadialDamage = bIsRadialDamage;
	DamageEffectParams.RadialDamageInnerRadius = RadialDamageInnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = RadialDamageOuterRadius;
	DamageEffectParams.RadialDamageOrigin = RadialDamageOrigin;
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation()-GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal().Rotation();
		Rotation.Pitch = 45.f;
		if (bOverridePitch)
		{
			Rotation.Pitch = OverridePitch;
		}
		bool bIsKnock = FMath::FRandRange(0.f,100.f) < DamageEffectParams.KnockChance ? true : false;
		if (bIsKnock&&!bOverrideKnockForce)
		{
			DamageEffectParams.KnockImpulseDirection = Rotation.Vector()*DamageEffectParams.KnockImpulseMagniture;
		}else
		{
			DamageEffectParams.KnockImpulseDirection = FVector::ZeroVector;
		}
		if (!bOverrideDeathForce)
		{
			DamageEffectParams.ImpulseDirection = GetAvatarActorFromActorInfo()->GetActorLocation()*DamageEffectParams.DeathImpulseMagniture;
		}
		
	}
	if (bOverrideKnockForce)
	{
		OverrideKnockForce.Normalize();
		FRotator Rotation = OverrideKnockForce.Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = OverridePitch;
		}
		DamageEffectParams.KnockImpulseDirection = Rotation.Vector() * DamageEffectParams.KnockImpulseMagniture;
	}
	if (bOverrideDeathForce)
	{
		OverrideDeathForce.Normalize();
		FRotator Rotation = OverrideDeathForce.Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = OverridePitch;
		}
		DamageEffectParams.ImpulseDirection = Rotation.Vector() * DamageEffectParams.DeathImpulseMagniture;
	}
	
	return DamageEffectParams;
}



float UAuraDamageGameplayAbility::GetDamage() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage> TaggedMontages) const
{
	if (TaggedMontages.Num()>0)
	{
		const int32 Length = TaggedMontages.Num()-1;
		const int32 Index = FMath::RandRange(0,Length);
		FTaggedMontage TargetMontage = TaggedMontages[Index];
		return TargetMontage;
	}
	return FTaggedMontage();
}
