// Mr.Wang


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoByTag(const FGameplayTag& Tag,bool bLogNotFound ) const
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag == Tag)
		{
			return AbilityInfo;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura,Error,TEXT("Tag [%s] not found in Ability Information on AbilityInfo [%s]"),*Tag.ToString(),*GetNameSafe(this));
	}
	return FAuraAbilityInfo();
}

