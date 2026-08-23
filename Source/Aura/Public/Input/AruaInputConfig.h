// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AruaInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly);
	const UInputAction* InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly);
	FGameplayTag InputTag = FGameplayTag();
};
/**
 * 
 */
UCLASS()
class AURA_API UAruaInputConfig : public UDataAsset
{
	GENERATED_BODY()
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool bLogNotFind = false) const;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraInputAction> AbilityInputActions;
};
