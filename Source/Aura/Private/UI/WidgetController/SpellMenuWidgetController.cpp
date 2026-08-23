// Mr.Wang


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();	
	SpellPointsDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag,int32 InLevel)
	{
		if (AbilityTag.MatchesTag(AbilityTag))
		{
			CurrentTag.StatusTag = StatusTag;
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetSpellDescriptionByAbilityTag(AbilityTag,Description,NextLevelDescription);
			OnButtonChangedSignature.Broadcast(StatusTag,GetAuraPS()->GetSpellPoints(),Description,NextLevelDescription);
		}
		if (AbilityInfo)
		{
			FAuraAbilityInfo Info= AbilityInfo->FindAbilityInfoByTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});
	
	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquipped);
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 NewPoint)
	{
		SpellPointsDelegate.Broadcast(NewPoint);
		CurrentSpellPoints = NewPoint;
		FString Description;
		FString NextLevelDescription;
		GetAuraASC()->GetSpellDescriptionByAbilityTag(CurrentTag.AbilityTag,Description,NextLevelDescription);
		OnButtonChangedSignature.Broadcast(CurrentTag.StatusTag,NewPoint,Description,NextLevelDescription);
	});
}

void USpellMenuWidgetController::ClickButtonInfo(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquippedSelection)
	{
		FGameplayTag Type = (AbilityInfo->FindAbilityInfoByTag(CurrentTag.AbilityTag)).AbilityType;
		OnStopEquippedDelegate.Broadcast(Type);
		bWaitForEquippedSelection = false;
	}
	
	FGameplayAbilitySpec* Spec = GetAuraASC()->GetActiveAbilitySpecFromTag(AbilityTag);
    FGameplayTag StatusTag = FAuraGameplayTags::Get().Ability_Status_Locked;
    if (Spec != nullptr)
    {
    	for (FGameplayTag Tag : Spec->GetDynamicSpecSourceTags())
    	{
    		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Status"))))
    		{
    			StatusTag = Tag;
    		}
    	}
   }
	CurrentTag.AbilityTag = AbilityTag;
	CurrentTag.StatusTag = StatusTag;
	int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	CurrentSpellPoints = SpellPoints;
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetSpellDescriptionByAbilityTag(AbilityTag,Description,NextLevelDescription);
	OnButtonChangedSignature.Broadcast(StatusTag, SpellPoints,Description,NextLevelDescription);
	
}

void USpellMenuWidgetController::SpellPointsButtonPressed()
{
	GetAuraASC()->ServerSpellPointsButtonPressed(CurrentTag.AbilityTag);
}

void USpellMenuWidgetController::EquippedButtonPressed()
{
	FGameplayTag Type = (AbilityInfo->FindAbilityInfoByTag(CurrentTag.AbilityTag)).AbilityType;
	OnEquippedDelegate.Broadcast(Type);
	bWaitForEquippedSelection = true;
	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusTagFromTag(CurrentTag.AbilityTag);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Ability_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetSlotTagFromTag(CurrentTag.AbilityTag);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitForEquippedSelection)
	{
		FGameplayTag Type = (AbilityInfo->FindAbilityInfoByTag(CurrentTag.AbilityTag)).AbilityType;
		OnStopEquippedDelegate.Broadcast(Type);
		bWaitForEquippedSelection = false;
	}
	
	CurrentTag.StatusTag = FAuraGameplayTags::Get().Ability_Status_Locked;
	CurrentTag.AbilityTag = FAuraGameplayTags::Get().Ability_None;
	
	OnButtonChangedSignature.Broadcast(CurrentTag.StatusTag,CurrentSpellPoints,FString(""),FString(""));
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& Slot, const FGameplayTag& PrevSlot)
{
	bWaitForEquippedSelection = false;
	//将原本的装备栏置为空
	FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	FAuraAbilityInfo Info;
	Info.AbilityTag = GameplayTags.Ability_None;
	Info.StatusTag = GameplayTags.Ability_Status_Unlocked;
	Info.InputTag = PrevSlot;
	AbilityInfoDelegate.Broadcast(Info);
	//将目标栏广播
	FAuraAbilityInfo TargetInfo = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
	TargetInfo.StatusTag = StatusTag;
	TargetInfo.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(TargetInfo);
	
	OnStopEquippedDelegate.Broadcast(AbilityInfo->FindAbilityInfoByTag(AbilityTag).AbilityType);
	OnSpellSelectedReassignedDelegate.Broadcast();
	GlobeDeselect();
	
}

void USpellMenuWidgetController::GlobeRowButtonPressed(const FGameplayTag& AbilityType, const FGameplayTag& Slot)
{
	if (!bWaitForEquippedSelection) return;
	const FGameplayTag SelectedAbilityType = AbilityInfo->FindAbilityInfoByTag(CurrentTag.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;
	
	GetAuraASC()->ServerEquippedAbility(CurrentTag.AbilityTag, Slot);
}
