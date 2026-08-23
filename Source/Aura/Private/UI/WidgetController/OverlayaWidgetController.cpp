// Mr.Wang


#include "UI/WidgetController/OverlayaWidgetController.h"

#include "NiagaraCommon.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Interaction/CombatInterface.h"
#include "Player/AuraPlayerState.h"

void UOverlayaWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
	
}

void UOverlayaWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this,&UOverlayaWidgetController::OnXPChanged);
	
	GetAuraPS()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel,bool bLevelUp)
	{
		OnLevelChangedDelegate.Broadcast(NewLevel,bLevelUp);
	});
	
	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this,&UOverlayaWidgetController::OnAbilityEquipped);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});
	if (GetAuraASC())
	{
		if (GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this,&UOverlayaWidgetController::BroadcastAbilityInfo);
		}
		
		
		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				
					if (Tag.MatchesTag(MessageTag))
					{
						//TODO:把标签广播给Widget上，使其显示在屏幕上
				
						const FUiWidgetRow* Row = GetDataTableRowByTag<FUiWidgetRow>(MessageWidgetDataTable,Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				
				}
			}	
	
		);
	}
	
	
}


void UOverlayaWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& Slot, const FGameplayTag& PrevSlot)
{
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
}

void UOverlayaWidgetController::OnXPChanged(int32 NewXp)
{
	ULevelUpInfo* LevelUpInformation = GetAuraPS()->LevelUpInformation;
	
	check(LevelUpInformation);
	int32 Level = LevelUpInformation->FindLevelForXP(NewXp);
	int32 MaxLevel = LevelUpInformation->LevelUpInfo.Num();
	
	if (Level<=MaxLevel&&Level>0)
	{
		const int32 LevelUpRequirements = LevelUpInformation->LevelUpInfo[Level].LevelUpRequirement;
		const int32 PreviousLevelRequirements = LevelUpInformation->LevelUpInfo[Level-1].LevelUpRequirement;
		
		const int32 DeltaLevelRequirement = LevelUpRequirements - PreviousLevelRequirements;
		const int32 XPForThisLevel = NewXp - PreviousLevelRequirements;
		
		const float Percent = static_cast<float>(XPForThisLevel)/DeltaLevelRequirement;
		OnXPPercentChangedDelegate.Broadcast(Percent);
	}
	
}

