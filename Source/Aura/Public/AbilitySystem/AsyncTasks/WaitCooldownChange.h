// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
class UAuraAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);
/**
 * 
 */
UCLASS(BlueprintType,meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	//创建两个委托用来当异步任务的两个执行引脚
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;
	
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;
	
	UFUNCTION(BlueprintCallable,meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTag);
	
	UFUNCTION(BlueprintCallable)
	void EndTask();
	
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	FGameplayTag CooldownTag;
	
	void CooldownTagChange(const FGameplayTag InCooldownTag,int32 NewCount);
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetAsc,const FGameplayEffectSpec& SpecApplied,FActiveGameplayEffectHandle ActiveEffectHandle);
};
