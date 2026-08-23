// Mr.Wang


#include "Abilities/AuraFireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actor/AuraProjectile.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetSpellDescription(int32 Level)
{
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCoolDown(Level);
	const int32 ScaleDamage = Damage.GetValueAtLevel(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			
			//Title
			"<Title>FIRE BOLT</>\n\n"
			//Level
			"<Small>Level is </>"
			"<Level>%d</>\n"
			
			//ManaCost
			"<Small>ManaCost is </>"
			"<ManaCost>%.1f</>\n"
			
			//CoolDown
			"<Small>Cooldown is </>"
			"<Cooldown>%.1f</>\n\n"
			
			"<Default>Launches a bolts of fire,exploding on impact and dealing:</> "
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to burn.</>"), 
			Level,
			ManaCost,
			Cooldown,
			ScaleDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			
			//Title
			"<Title>FIRE BOLT</>\n\n"
			//Level
			"<Small>Level is </>"
			"<Level>%d</>\n"
			
			//ManaCost
			"<Small>ManaCost is </>"
			"<ManaCost>%.1f</>\n"
			
			//CoolDown
			"<Small>Cooldown is </>"
			"<Cooldown>%.1f</>\n\n"
			
			//BoltNums
			"<Default>Launches %d bolts of fire,exploding on impact and dealing:</> "
			
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to burn.</>"), 
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level,ProjectileNum),
			ScaleDamage);
	}
	
}
FString UAuraFireBolt::GetSpellNextLevelDescription(int32 Level)
{
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCoolDown(Level);
	const int32 ScaleDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
			
			//Title
			"<Title>NEXT LEVEL</>\n\n"
			//Level
			"<Small>Level is </>"
			"<Level>%d</>\n"
			
			//ManaCost
			"<Small>ManaCost is </>"
			"<ManaCost>%.1f</>\n"
			
			//CoolDown
			"<Small>Cooldown is </>"
			"<Cooldown>%.1f</>\n\n"
			
			//BoltNums
			"<Default>Launches %d bolts of fire,exploding on impact and dealing:</> "
			
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to burn.</>"), 
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level,ProjectileNum),
			ScaleDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,AActor* HomingTarget,bool bOverridePitch, float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
	{
		return;
	}
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(),SocketTag);
	FRotator Rotation = (ProjectileTargetLocation-SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	const FVector Forward = Rotation.Vector();
	const int32 EffectiveProjectileNums = FMath::Min(GetAbilityLevel(),MaxNumProjectile);
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpaceRotator(Forward,FVector::UpVector,ProjectileSpread,EffectiveProjectileNums);
	
	for (FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromDefaultClass(nullptr);
		if (IsValid(HomingTarget)&&HomingTarget->Implements<UCombatInterface>())//判断是否是角色
		{
			//HomingTargetComponent是一个弱指针，目的是为了不想因为追踪的引用而导致无法垃圾回收。
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}else
		{
			Projectile->HomingSceneTargetComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingSceneTargetComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingSceneTargetComponent;
		}
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingSpeedMin,HomingSpeedMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bIsHoming;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}

float UAuraFireBolt::GetCoolDown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CoolDownEffect = GetCooldownGameplayEffect())
	{
		CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel,Cooldown);
	}
	return Cooldown;
	
}

float UAuraFireBolt::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	if (UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Modify : CostEffect->Modifiers)
		{
			if (Modify.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				Modify.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel,ManaCost);
				break;
			}
		}
	}
	return ManaCost;
	
}
