// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BaseGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBaseGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category = "Input")
	FGameplayTag StartupInputTag;
	
	virtual FString GetSpellDescription(int32 Level);
	virtual FString GetSpellNextLevelDescription(int32 Level);
	static FString GetLockedDescription(int32 Level);
	
};
