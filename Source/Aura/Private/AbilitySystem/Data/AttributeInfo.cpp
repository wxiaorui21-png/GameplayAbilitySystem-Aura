// Mr.Wang


#include "AbilitySystem/Data/AttributeInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(Tag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura,Error,TEXT("Can't find info for Attribute [%s] on AttributeInfo [%s]."),*Tag.ToString(),*GetNameSafe(this));
	}
	
	return FAuraAttributeInfo();
}
