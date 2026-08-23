// Mr.Wang


#include "Abilities/AuraElectrocute.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraElectrocute::GetSpellDescription(int32 Level)
{
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCoolDown(Level);
	const int32 ScaleDamage = Damage.GetValueAtLevel(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			
			//Title
			"<Title>ELECTROCUTE</>\n\n"
			//Level
			"<Small>Level is </>"
			"<Level>%d</>\n"
			
			//ManaCost
			"<Small>ManaCost is </>"
			"<ManaCost>%.1f</>\n"
			
			//CoolDown
			"<Small>Cooldown is </>"
			"<Cooldown>%.1f</>\n\n"
			
			"<Default>Launches a beam lightning,exploding on impact and dealing:</> "
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to stun.</>"), 
			Level,
			ManaCost,
			Cooldown,
			ScaleDamage);
	}
	else
	{
		return FString::Printf(TEXT(
			
			//Title
			"<Title>ELECTROCUTE</>\n\n"
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
			"<Default>Launches %d beam of lightning to target,exploding on impact and dealing:</> "
			
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to stun.</>"), 
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumBeam),
			ScaleDamage);
	}
}

FString UAuraElectrocute::GetSpellNextLevelDescription(int32 Level)
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
			"<Default>Launches %d beam of lightning to target,exploding on impact and dealing:</> "
			
			//Damage
			"<Damage>%d</> "
			
			"<Default>fire damage with a chance to stun.</>"), 
			Level,
			ManaCost,
			Cooldown,
			FMath::Min(Level,MaxNumBeam),
			ScaleDamage);
}

float UAuraElectrocute::GetCoolDown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CoolDownEffect = GetCooldownGameplayEffect())
	{
		CoolDownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel,Cooldown);
	}
	return Cooldown;
	
}

float UAuraElectrocute::GetManaCost(float InLevel) const
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

