// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32/*StateValue*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChangeSingture, int32/*StateValue*/,bool /*bLevelUp*/);
/**
 * 
 */
class UAbilitySystemComponent;
class UAttributeSet;
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;};
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInformation;
	
	FOnPlayerStateChanged OnXPChangedDelegate;
	FOnLevelChangeSingture OnLevelChangedDelegate;
	FOnPlayerStateChanged OnAttributePointsChangedDelegate;
	FOnPlayerStateChanged OnSpellPointsChangedDelegate;
	
	FORCEINLINE int32 GetPlayerLevel() const{return Level;}
	FORCEINLINE int32 GetXP() const {return XP;}
	FORCEINLINE int32 GetAttributePoints() const{return AttributePoints;}
	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;}
	
	void AddToXp(int32 InXp);
	void AddToLevel(int32 InLevel);
	void AddToAttributePoints(int32 InAttributePoints);
	void AddToSpellPoints(int32 InSpellPoints);
	
	void SetLevel(int32 InLevel);
	void SetXP(int32 InXp);
	void SetAttributePoints(int32 InAttributePoints);
	void SetSpellPoints(int32 InSpellPoints);
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRep_XP)
	int32 XP = 0;
	
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 0;
	
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 0;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
};
