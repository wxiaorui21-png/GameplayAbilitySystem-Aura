// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/BaseGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();
	
	UFUNCTION(BlueprintPure,Category = "Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();
	
	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	int32 NumMinions = 5;
	
	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;
	
	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	float MaxSpawnDistance = 250.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	float MinSpawnDistance = 50.0f;
	
	UPROPERTY(EditDefaultsOnly,Category = "Summoning")
	float SpawnSpread = 90;
};
