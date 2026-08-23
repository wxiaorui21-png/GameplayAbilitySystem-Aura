// Mr.Wang


#include "Abilities/AuraFireBlast.h"
#include "Actor/AuraFireBall.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraFireBlast::GetSpellDescription(int32 Level)
{
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCoolDown(Level);
	const int32 ScaleDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
			
			//Title
			"<Title>FIRE BLAST</>\n\n"
			//Level
			"<Small>Level is </>"
			"<Level>%d</>\n"
			
			//ManaCost
			"<Small>ManaCost is </>"
			"<ManaCost>%.1f</>\n"
			
			//CoolDown
			"<Small>Cooldown is </>"
			"<Cooldown>%.1f</>\n\n"
			//Balls Num
			"<Default>Launches %d</> "
			"<Default> fire balls in all direction,each coming back</>"
			"<Default> exploding upon return,causing"
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to burn.</>"), 
			Level,
			ManaCost,
			Cooldown,
			MaxNumFires,
			ScaleDamage);
}

FString UAuraFireBlast::GetSpellNextLevelDescription(int32 Level)
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
			//Balls Num
			"<Default>Launches %d</> "
			"<Default> fire balls in all direction,each coming back</>"
			"<Default> exploding upon return,causing"
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to burn.</>"), 
			Level,
			ManaCost,
			Cooldown,
			MaxNumFires,
			ScaleDamage);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpaceRotator(Forward,FVector::UpVector,360.f,MaxNumFires);
	TArray<AAuraFireBall*> FireBalls;
	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		
		FireBall->DamageEffectParams = MakeDamageEffectParamsFromDefaultClass(nullptr);
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());
		FireBall->ExplosionEffectParams = MakeDamageEffectParamsFromDefaultClass(nullptr);
		FireBalls.Add(FireBall);
		FireBall->FinishSpawning(SpawnTransform);
	}
	
	return FireBalls;
}

float UAuraFireBlast::GetCoolDown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CoolDownEffect = GetCooldownGameplayEffect())
	{
		CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel,Cooldown);
	}
	return Cooldown;
}

float UAuraFireBlast::GetManaCost(float InLevel) const
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


