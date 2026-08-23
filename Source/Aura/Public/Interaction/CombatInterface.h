// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "CombatInterface.generated.h"

class UAbilitySystemComponent;
class UNiagaraSystem;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDebuffGetASC,UAbilitySystemComponent*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath,AActor*,Character);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageSingture,float /*Damage*/)

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSound = nullptr;
	
};
// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UAnimMontage;
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector& FacingTarget);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactMontage();
	
	virtual void Die(const FVector& ImpulseDir) = 0;
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	bool IsDead() const;
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	AActor* GetAvatar();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	FTaggedMontage GetTagMontageByTag(const FGameplayTag& MontageTag);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	int32 GetMinionCount();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void AddMinionsCount(int32 AddCount);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	ECharacterClass GetCharacterClass();
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetInShockLoop(bool bIsLoop);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetInBeingShockLoop(bool bIsLoop);
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	bool GetBeingShockLoop();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	USkeletalMeshComponent* GetWeaponComponent();
	
	virtual FOnDebuffGetASC& GetOnDebuffGetASC() = 0;
	
	virtual FOnDeath& GetOnDeath() = 0;
	
	virtual FOnDamageSingture& GetOnDamageSingure() = 0;
};
