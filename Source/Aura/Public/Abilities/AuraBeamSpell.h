// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseData(const FHitResult& MouseHit);
	
	UFUNCTION(BlueprintCallable)
	void StoreOwningVariable();
	
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamSpellTarget);
	
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTarget(TArray<AActor*>& OnOutAdditionalTargets);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);
	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);
protected:
	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	FVector MouseHitLocation;
	
	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;
	
	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadWrite,Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
};
