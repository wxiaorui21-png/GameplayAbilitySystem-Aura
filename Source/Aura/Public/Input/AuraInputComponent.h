// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AruaInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	
	template<class UserClass,typename PressedFuncType,typename ReleasedFuncType,typename HoldFuncType>
	void BindAbilityInput(UAruaInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleasedFuncType ReleasedFunc,HoldFuncType HoldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HoldFuncType>
void UAuraInputComponent::BindAbilityInput(UAruaInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HoldFuncType HoldFunc)
{
	check(InputConfig);
	
	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction&&Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object,PressedFunc,Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object,ReleasedFunc,Action.InputTag);
			}
			if (HoldFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object,HoldFunc,Action.InputTag);
			}
		}
	}
}
