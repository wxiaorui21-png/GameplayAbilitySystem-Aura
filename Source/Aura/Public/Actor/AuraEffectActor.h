// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActiveGameplayEffectHandle.h"
#include "AuraEffectActor.generated.h"


class UAbilitySystemComponent;
struct FActiveGameplayEffectHandle;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	Eap_ApplyOnOverlay,
	Eap_ApplyOnEndOverlay,
	Eap_DoNoApply
};

UENUM(BlueprintType)
enum class EEffectRemovePolicy : uint8
{
	Erp_RemoveOnEndOverlay,
	Erp_DoNoRemove
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass);
	
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	bool bDestroyOnEffectApplication = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	bool bApplyEffectToEnemy = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::Eap_DoNoApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::Eap_DoNoApply;;
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::Eap_DoNoApply;;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	EEffectRemovePolicy InfiniteEffectRemovePolicy = EEffectRemovePolicy::Erp_RemoveOnEndOverlay;
	
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*> ActiveEffectHandles; 
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Applied Effect")
	float ActorLevel = 1.f;
};
