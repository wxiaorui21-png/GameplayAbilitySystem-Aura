// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"

#include "SpellMenuWidgetController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FButtonChangedSignature, const FGameplayTag&, StatusTag, int32, SpellPoint,FString,Description,FString,NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquippedSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpellSelectedReassigned);
struct FPreAbilityTag
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 
 */

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
	UFUNCTION(BlueprintCallable)
	void ClickButtonInfo(const FGameplayTag& AbilityTag);
	
	UFUNCTION(BlueprintCallable)
	void SpellPointsButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void EquippedButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();
	
	void OnAbilityEquipped(const FGameplayTag& AbilityTag,const FGameplayTag& StatusTag, const FGameplayTag& Slot, const FGameplayTag& PrevSlot);
	
	UFUNCTION(BlueprintCallable)
	void GlobeRowButtonPressed(const FGameplayTag& AbilityType,const FGameplayTag& Slot);
	
	UPROPERTY(BlueprintAssignable,Category = "Gas|Attributes")
	FOnPointsSingnature SpellPointsDelegate;
	
	UPROPERTY(BlueprintAssignable,Category = "Gas|Attributes")
	FButtonChangedSignature OnButtonChangedSignature;
	
	UPROPERTY(BlueprintAssignable,Category = "Gas|Attributes")
	FWaitForEquippedSignature OnEquippedDelegate;
	
	UPROPERTY(BlueprintAssignable,Category = "Gas|Attributes")
	FWaitForEquippedSignature OnStopEquippedDelegate;
	
	UPROPERTY(BlueprintAssignable,Category = "Gas|Attributes")
	FSpellSelectedReassigned OnSpellSelectedReassignedDelegate;
	
	bool bWaitForEquippedSelection = false;
	
	FPreAbilityTag CurrentTag = {FAuraGameplayTags::Get().Ability_None,FAuraGameplayTags::Get().Ability_Status_Locked};
	int32 CurrentSpellPoints = 0;
	
private:
	
	FGameplayTag SelectedSlot = FGameplayTag();
};
