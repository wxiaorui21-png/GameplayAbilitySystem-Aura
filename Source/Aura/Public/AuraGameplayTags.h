// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
/**
 * AuraGameplayTags只存在一个，因此可以作为单列模式存在。
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get(){	return GameplayTags; }
	static void InitializeNativeGameplayTags();
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;
	
	FGameplayTag Attributes_Meta_IncomingXP;
	
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;
	
	
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;
	
	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_Arcane;
	FGameplayTag Debuff_Physical;
	
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Frequency;
	FGameplayTag Debuff_Duration;
	
	FGameplayTag Ability_Attack;
	FGameplayTag Ability_Summon;
	FGameplayTag Ability_None;
	
	FGameplayTag Ability_HitReact;
	
	FGameplayTag Ability_Status_Locked;
	FGameplayTag Ability_Status_Eligible;
	FGameplayTag Ability_Status_Unlocked;
	FGameplayTag Ability_Status_Equipped;
	
	FGameplayTag Ability_Type_Offensive;
	FGameplayTag Ability_Type_Passive;
	FGameplayTag Ability_Type_None;
	
	FGameplayTag Ability_Passive_HaloOfProtection;
	FGameplayTag Ability_Passive_LifeSiphon;
	FGameplayTag Ability_Passive_ManaSiphon;
	
	FGameplayTag Ability_Fire_FireBlot;
	FGameplayTag Ability_Fire_FireBlast;
	FGameplayTag Ability_Lightning_Electrocute;
	FGameplayTag Ability_Arcane_Shards;
	
	FGameplayTag Cooldown_Fire_FireBlot;
	
	FGameplayTag Combat_Socket_Weapon;
	FGameplayTag Combat_Socket_RightHand;
	FGameplayTag Combat_Socket_LeftHand;
	FGameplayTag Combat_Socket_Tail;
	
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_CursorTrace;
	
	TMap<FGameplayTag,FGameplayTag> DamageTypesToResistance;
	TMap<FGameplayTag,FGameplayTag> DamageTypesToDebuff;
	
	FGameplayTag Effects_HitReact;
	
	FGameplayTag GameplayCue_FireBlast;
private:
	static FAuraGameplayTags GameplayTags;
};