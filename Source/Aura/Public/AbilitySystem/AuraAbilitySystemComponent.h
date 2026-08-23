// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "AuraAbilitySystemComponent.generated.h"


class ULoadScreenSaveGame;
//class UAuraAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged,const FGameplayTag& /*AbilityTag*/,const FGameplayTag& /*StatusTag*/,int32 /*AbilityLevel*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped,const FGameplayTag& /*AbilityTag*/,const FGameplayTag& /*StatusTag*/,const FGameplayTag& /*Slot*/,const FGameplayTag& /*PrevSlot*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility,const FGameplayTag& /*AbilityTag*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FPassiveNiagaraComponent,const FGameplayTag& /*AbilityTag*/,bool /*bActivate*/)
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilitySystemInfoSet();//使用来绑定回调函数，在AbilitySystemInfo设置完成后
	
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChangedDelegate;
	FAbilityEquipped AbilityEquippedDelegate;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FPassiveNiagaraComponent PassiveNiagaraComponent;
	
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities );
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities );
	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);
	
	bool bStartupAbilitiesGiven = false;
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetStatusTagFromTag(const FGameplayTag& Tag);
	FGameplayTag GetSlotTagFromTag(const FGameplayTag& Tag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveBySpec(const FGameplayAbilitySpec& Spec);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	static void AssignAbilityToSlot(FGameplayAbilitySpec& Spec,const FGameplayTag& Slot);
	
	FGameplayAbilitySpec* GetActiveAbilitySpecFromTag(const FGameplayTag& Tag);
	
	static void ClearSlot(FGameplayAbilitySpec* Spec);
	void ClearAllAbilitiesOfSlot(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec,const FGameplayTag& Slot);
	
	UFUNCTION(NetMulticast,Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag,bool bActivate);
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	
	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	UFUNCTION(Server,Reliable)
	void ServerSpellPointsButtonPressed(const FGameplayTag& AbilityTag);
	
	//当客户端点击时，确保通知服务端执行这个操作
	UFUNCTION(Server,Reliable)
	void ServerEquippedAbility(const FGameplayTag& AbilityTag,const FGameplayTag& Slot);
	
	void UpdateAbilityStates(int32 Level);
	
	bool GetSpellDescriptionByAbilityTag(const FGameplayTag& AbilityTag,FString& OutDescription,FString& OutNextDescription);
	
protected:
	
	virtual void OnRep_ActivateAbilities() override;
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
	
	//服务器通知客户端
	UFUNCTION(Client, Reliable)
	void ClientAbilityEquipped(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,const FGameplayTag& Slot,const FGameplayTag& PrevSlot);
	
	UFUNCTION(Client,Reliable)
	void ClientAbilityStatusChanged(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 InLevel);
};


