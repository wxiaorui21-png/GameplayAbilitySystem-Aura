// Mr.Wang


#include "Input/AruaInputConfig.h"


const UInputAction* UAruaInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFind) const
{
	for (const FAuraInputAction Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.MatchesTagExact(InputTag))
			return Action.InputAction;
	}
	if (bLogNotFind)
	{
		UE_LOG(LogTemp,Error,TEXT("Can't find AbilityInputAction for InputTag [%s], on InputConfig [%s]"),*InputTag.ToString(),*GetNameSafe(this));
	}
	return nullptr;
}
