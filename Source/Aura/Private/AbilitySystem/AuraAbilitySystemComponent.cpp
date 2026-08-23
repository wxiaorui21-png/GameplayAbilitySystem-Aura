// Mr.Wang


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "Abilities/BaseGameplayAbility.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilitySystemInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);//绑定函数
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities) 
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	
	}
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities) 
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		if (const UBaseGameplayAbility* AuraAbility = Cast<UBaseGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData)
{
	for (const FSavedAbility& Data : SaveData->SavedAbilities)
	{
		TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;
		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass,Data.AbilityLevel);
		
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilitySlot);
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilityStatus);
		
		if (Data.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Ability_Type_Offensive))
		{
			GiveAbility(LoadedAbilitySpec);	
		}else if (Data.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Ability_Type_Passive))
		{
			if (Data.AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Equipped))
			{
				GiveAbilityAndActivateOnce(LoadedAbilitySpec);
			}else
			{
				GiveAbility(LoadedAbilitySpec);	
			}
			
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				PRAGMA_DISABLE_DEPRECATION_WARNINGS
				const FGameplayAbilityActivationInfo& ActivationInfo = Instances.IsEmpty()
					? AbilitySpec.ActivationInfo
					: Instances.Last()->GetCurrentActivationInfoRef();
				PRAGMA_ENABLE_DEPRECATION_WARNINGS
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					AbilitySpec.Handle,
					ActivationInfo.GetActivationPredictionKey(),
					ScopedPredictionKey);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
		
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)&&AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			const FGameplayAbilityActivationInfo& ActivationInfo = Instances.IsEmpty()
				? AbilitySpec.ActivationInfo
				: Instances.Last()->GetCurrentActivationInfoRef();
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				AbilitySpec.Handle,
				ActivationInfo.GetActivationPredictionKey(),
				ScopedPredictionKey);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);//先将这个对象锁住，以防在使用时由于某些原因导致技能状态修改。
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura,Error,TEXT("Failed to excute delegate in %hs"),__FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag StatusTag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromTag(const FGameplayTag& Tag)
{
	if (FGameplayAbilitySpec* Spec = GetActiveAbilitySpecFromTag(Tag))
	{
		return GetStatusTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetSlotTagFromTag(const FGameplayTag& Tag)
{
	if (FGameplayAbilitySpec* Spec = GetActiveAbilitySpecFromTag(Tag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(Slot))
		{
			return false;
		}
	}
	return true;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(Slot))
		{
			return &Spec;
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveBySpec(const FGameplayAbilitySpec& Spec)
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	return Info.AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Ability_Type_Passive);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

void UAuraAbilitySystemComponent::AssignAbilityToSlot(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);
	Spec.GetDynamicSpecSourceTags().AddTag(Slot);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetActiveAbilitySpecFromTag(const FGameplayTag& Tag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag AbilityTag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (AbilityTag.MatchesTag(Tag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const FGameplayTag SlotTag = GetInputTagFromSpec(*Spec);
	Spec->GetDynamicSpecSourceTags().RemoveTag(SlotTag);
}

void UAuraAbilitySystemComponent::ClearAllAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			ClearSlot(&AbilitySpec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	for (const FGameplayTag& Tag : Spec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
	bool bActivate)
{
	PassiveNiagaraComponent.Broadcast(AbilityTag,bActivate);
}

void UAuraAbilitySystemComponent::ServerSpellPointsButtonPressed_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetActiveAbilitySpecFromTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(),-1);
		}
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTag Status = GetStatusTagFromSpec(*Spec);
		if (Status.MatchesTagExact(GameplayTags.Ability_Status_Eligible))
		{
			Spec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Ability_Status_Eligible);
			Spec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Ability_Status_Unlocked);
			Status = GameplayTags.Ability_Status_Unlocked;
			
		}else if (Status.MatchesTagExact(GameplayTags.Ability_Status_Equipped)||Status.MatchesTagExact(GameplayTags.Ability_Status_Unlocked))
		{
			Spec->Level += 1;
		}
		MarkAbilitySpecDirty(*Spec);
		ClientAbilityStatusChanged(AbilityTag,Status,Spec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerEquippedAbility_Implementation(const FGameplayTag& AbilityTag,const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* Spec = GetActiveAbilitySpecFromTag(AbilityTag))
	{
		const FGameplayTag PrevSlot = GetInputTagFromSpec(*Spec);
		const FGameplayTag Status = GetStatusTagFromSpec(*Spec);
		const bool bStatusIsValid = Status == FAuraGameplayTags::Get().Ability_Status_Equipped||Status == FAuraGameplayTags::Get().Ability_Status_Unlocked;
		if (bStatusIsValid)
		{
			
			if (!SlotIsEmpty(Slot))
			{
				FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{   
					//判断该能力是否与要装备的能力相同，若相同则提前返回。
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientAbilityEquipped(AbilityTag,Status,Slot,PrevSlot);
						return;
					}
				}
				//如果是被动技能需要先结束该被动技能
				if (IsPassiveBySpec(*SpecWithSlot))
				{
					MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot),false);
					DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
				}
				ClearSlot(SpecWithSlot);
			}
			if (!AbilityHasAnySlot(*Spec))
			{
				if (IsPassiveBySpec(*Spec))
				{
					TryActivateAbility(Spec->Handle);
					MulticastActivatePassiveEffect(AbilityTag,true);
				}
				//赋予已装备标签用来存档使用
				Spec->GetDynamicSpecSourceTags().RemoveTag(GetStatusTagFromSpec(*Spec));
				Spec->GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
			}
			AssignAbilityToSlot(*Spec,Slot);
			MarkAbilitySpecDirty(*Spec);
		}
		
		ClientAbilityEquipped(AbilityTag,Status,Slot,PrevSlot);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStates(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid()) return;
		if (Info.LevelRequirement > Level) continue;
		if (GetActiveAbilitySpecFromTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability,1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Ability_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientAbilityStatusChanged_Implementation(Info.AbilityTag,FAuraGameplayTags::Get().Ability_Status_Eligible,1);
		}
	}
}

bool UAuraAbilitySystemComponent::GetSpellDescriptionByAbilityTag(const FGameplayTag& AbilityTag,
	FString& OutDescription, FString& OutNextDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetActiveAbilitySpecFromTag(AbilityTag))
	{
		if (UBaseGameplayAbility* Ability = Cast<UBaseGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = Ability->GetSpellDescription(AbilitySpec->Level);
			OutNextDescription = Ability->GetSpellNextLevelDescription(AbilitySpec->Level+1);
			return true;
		}
	}
	const UAbilityInfo* Info = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Ability_None))
	{
		OutDescription = FString();
	}
	else
	{
		OutDescription = UBaseGameplayAbility::GetLockedDescription(Info->FindAbilityInfoByTag(AbilityTag).LevelRequirement);
	}
	OutNextDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor())>0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(),AttributeTag,Payload);
	
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(),-1);
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
	
}

void UAuraAbilitySystemComponent::ClientAbilityEquipped_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, const FGameplayTag& Slot, const FGameplayTag& PrevSlot)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag,StatusTag,Slot,PrevSlot);
}


void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	//想使传递到Widget上，但是由于两者是单项传递的，因此需要设置一个广播
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);//把标签全部放到TagContainer里面
	
	EffectAssetTags.Broadcast(TagContainer);
	
}
void UAuraAbilitySystemComponent::ClientAbilityStatusChanged_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag,int32 InLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag,InLevel);
}
