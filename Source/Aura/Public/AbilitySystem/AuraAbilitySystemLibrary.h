// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"

class ULoadScreenSaveGame;
struct FDamageEffectParams;
class UAbilityInfo;
class AAuraHUD;
struct FWidgetControllerParams;
class USpellMenuWidgetController;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayaWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/*
	 * AbilitySystem Default Function
	 */
	UFUNCTION(BlueprintPure,Category = "AbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& WidgetParams,AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure,Category = "AbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static UOverlayaWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure,Category = "AbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure,Category = "AbilitySystemLibrary|WidgetController",meta = (DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttribute(ECharacterClass CharacterClass,float Level,const UObject* WorldContextObject,UAbilitySystemComponent* Asc);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributeFromSave(const UObject* WorldContextObject,UAbilitySystemComponent* Asc,ULoadScreenSaveGame* SaveObject);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent* Asc,ECharacterClass CharacterClass);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);
	
	
	/*
	 * Effect Context Get function
	 */
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockHit(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetImpulseDirection(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetKnockImpulseDirection(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& ContextHandle);
	/*
	 * Effect Context Set function
	 */
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetBlockHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,bool bIsBlockHit);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,bool bIsCriticalHit);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,bool bIsSuccessfulDebuff);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,float InDebuffDamage);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,float InDebuffDuration);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,float InDebuffFrequency);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,FGameplayTag& InDamageType);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetImpulseDirection(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,const FVector& InImpulseDirection);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetKnockImpulseDirection(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,const FVector& InKnockImpulseDirection);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,bool bIsRadiaDamage);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,float InRadialDamageInnerRadius);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,float InRadialDamageOuterRadius);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,const FVector& InRadialDamageOrigin);
	
	
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLiveCharacterWithInRadius(const UObject* WorldContextObject,TArray<AActor*>& OutOverlappingActors,const TArray<AActor*>& ActorToIgnore,const float Radius,const FVector& SphereLocation);

	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetCloseActorsFromLocation(int32 NumsTarget,const TArray<AActor*>& OverlappingActors,TArray<AActor*>& OutAdditionalActors,const FVector& Location);
	
	UFUNCTION(BlueprintPure,BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor,AActor* SecondActor);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);
	
	UFUNCTION(BlueprintPure,BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpaceRotator(const FVector& Forward,const FVector& Axis,float SpreadAngle,int32 NumsRotator);
	
	UFUNCTION(BlueprintPure,BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotationVector(const FVector& Forward,const FVector& Axis,float SpreadAngle,int32 NumsVector);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject,ECharacterClass CharacterClass,int32 Level);
	
	/*
	 *Damage Effect Params
	 */
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|Damage Effect Params")
	static void SetIsRadialDamageParams(UPARAM(ref) FDamageEffectParams& DamageEffectParams,bool bIsRadiaDamage,float InnerRadius,float OuterRadius,FVector Origin);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|Damage Effect Params")
	static void SetKnockBackDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams,FVector KnockBackDirection,float Magnitude = 0);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|Damage Effect Params")
	static void SetDeathImpulseDirection(UPARAM(ref) FDamageEffectParams& DamageEffectParams,FVector DeathImpulseDirection,float Magnitude = 0);
	
	UFUNCTION(BlueprintCallable,Category = "AuraAbilitySystemLibrary|Damage Effect Params")
	static void SetAbilitySystemComponent(UPARAM(ref) FDamageEffectParams& DamageEffectParams,UAbilitySystemComponent* AbilitySystemComponent);
};
