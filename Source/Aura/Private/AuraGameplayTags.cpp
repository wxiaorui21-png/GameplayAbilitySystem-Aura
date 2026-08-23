// Mr.Wang


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"


FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 *Primary Attributes
	 */
	GameplayTags.Attributes_Primary_Strength= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increase physical damage"));
	GameplayTags.Attributes_Primary_Intelligence= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increase magical damage"));
	GameplayTags.Attributes_Primary_Resilience= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increase Armor and Armor Penetration"));
	GameplayTags.Attributes_Primary_Vigor= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increase Health"));
	
	/*
	 *Secondary Attributes
	 */
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken,improve Block Chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_CriticalHitChance= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_HealthRegeneration= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_ManaRegeneration= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_MaxHealth= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	GameplayTags.Attributes_Secondary_MaxMana= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Ignore Percentage of Enemy Armor,increase Critical Hit Chance"));
	
	
	/*
	 *Input Actions
	 */
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for Left Mouse Button")
		);
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for Right Mouse Button")
		);
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("Input Tag for 1 key")
		);
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("Input Tag for 2 key")
		);
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("Input Tag for 3 key")
		);
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("Input Tag for 4 key")
		);
	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.1"),
		FString("Input Tag for Passive 1 key")
	);
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.2"),
		FString("Input Tag for Passive 2 key")
	);

	/*
	 *Damage Types
	 */
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
		);
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Damage Type Fire")
	);
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lightning"),
		FString("Damage Type Lightning")
	);
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Damage Type Arcane")
	);
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Damage Type Physical")
	);
	
	/*
	 *Resistance Attributes
	 */
	GameplayTags.Attributes_Resistance_Fire= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Fire"),
		FString("Resistance to Damage Fire"));
	GameplayTags.Attributes_Resistance_Lightning= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Lightning"),
		FString("Resistance to Damage Lightning"));
	GameplayTags.Attributes_Resistance_Arcane= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Arcane"),
		FString("Resistance to Damage Arcane"));
	GameplayTags.Attributes_Resistance_Physical= UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Resistance.Physical"),
		FString("Resistance to Damage Physical"));
	
	/*
	 *Meta Attributes
	 */
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Meta Incoming XP")
	);
	/*
	 * Map of Damage Types to Resistance
	 */
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Arcane,GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Fire,GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Lightning,GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistance.Add(GameplayTags.Damage_Physical,GameplayTags.Attributes_Resistance_Physical);
	

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effects.HitReact"),
	FString("Effects.HitReact")
		);
	
	/*
	 *Abilities
	 */
	GameplayTags.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack"),
		FString("Attack Base Tag")
	);
	GameplayTags.Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Summon"),
		FString("Shaman Summon Tag")
	);
	GameplayTags.Ability_Fire_FireBlot = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Fire.FireBlot"),
		FString("Fire Blot Tag")
	);
	GameplayTags.Ability_Fire_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Fire.FireBlast"),
		FString("Fire Blast Tag")
	);
	GameplayTags.Ability_Lightning_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Lightning.Electrocute"),
		FString("Lightning Electrocute Tag")
	);
	GameplayTags.Ability_Arcane_Shards = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Arcane.Shards"),
		FString("Shards Electrocute Tag")
	);
	GameplayTags.Ability_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.HitReact"),
		FString("HitReact Tag")
	);
	GameplayTags.Ability_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Locked"),
		FString("Ability Status is Locked")
	);
	GameplayTags.Ability_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Eligible"),
		FString("Ability Status is Eligible")
	);
	GameplayTags.Ability_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Unlocked"),
		FString("Ability Status is Unlocked")
	);
	GameplayTags.Ability_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Status.Equipped"),
		FString("Ability Status is Equipped")
	);
	GameplayTags.Ability_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Type.Offensive"),
		FString("Ability Type is Offensive")
	);
	GameplayTags.Ability_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Type.Passive"),
		FString("Ability Type is Passive")
	);
	GameplayTags.Ability_Type_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Type.None"),
		FString("Ability Type is None")
	);
	GameplayTags.Ability_None = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.None"),
		FString("Ability Type is None")
	);
	
	/*Passive Spell*/
	GameplayTags.Ability_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Passive.HaloOfProtection"),
		FString("Abilities.Passive.HaloOfProtection")
		);
	GameplayTags.Ability_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Passive.LifeSiphon"),
		FString("Abilities.Passive.LifeSiphon")
		);
	GameplayTags.Ability_Passive_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Passive.ManaSiphon"),
		FString("Abilities.Passive.ManaSiphon")
		);
	
	/*Debuff Tag*/
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Burn"),
		FString("Debuff Type is Burn")
	);
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Stun"),
		FString("Debuff Type is Stun")
	);
	GameplayTags.Debuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Arcane"),
		FString("Debuff Type is Arcane")
	);
	GameplayTags.Debuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Physical"),
		FString("Debuff Type is Physical")
	);
	
	/*Debuff Property*/
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Chance"),
		FString("Debuff Chance")
	);
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Damage"),
		FString("Debuff Damage")
	);
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Frequency"),
		FString("Debuff  Frequency")
	);
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Duration"),
		FString("Debuff Duration")
	);
	
	/*
	 * Map of Damage Types to Debuffs
	 */
	GameplayTags.DamageTypesToDebuff.Add(GameplayTags.Damage_Arcane,GameplayTags.Debuff_Arcane);
	GameplayTags.DamageTypesToDebuff.Add(GameplayTags.Damage_Fire,GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypesToDebuff.Add(GameplayTags.Damage_Lightning,GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuff.Add(GameplayTags.Damage_Physical,GameplayTags.Debuff_Physical);
	
	/*
	 *Cooldown Tag
	 */
	GameplayTags.Cooldown_Fire_FireBlot = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBlot"),
		FString("Cooldown Fire Blot Tag")
	);
	
	/*
	 *Weapon Socket
	 */
	GameplayTags.Combat_Socket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Combat.Socket.Weapon"),
		FString("Weapon")
	);
	GameplayTags.Combat_Socket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Combat.Socket.RightHand"),
		FString("RightHand")
	);
	GameplayTags.Combat_Socket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Combat.Socket.LeftHand"),
		FString("LeftHand")
	);
	GameplayTags.Combat_Socket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Combat.Socket.Tail"),
		FString("Tail")
	);
	/*
	 * Montage Tags
	 */
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("Attack1")
	);
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.2"),
		FString("Attack2")
	);
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.3"),
		FString("Attack3")
	);
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.4"),
		FString("Attack4")
	);  
	
	/*
	 * PlayerBlock Tags
	 */
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.CursorTrace"),
		FString("Block Trace under the Cursor")	
	);
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputPressed"),
		FString("Block Input Pressed Callback for Input")	
	);
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputReleased"),
		FString("Block Input Released Callback for Input")	
	);
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Block.InputHeld"),
		FString("Block Input Held Callback for Input")	
	);
	
	GameplayTags.GameplayCue_FireBlast = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayCue.FireBlast"),
		FString("GameplayCue.FireBlast")	
	);
}