// Mr.Wang


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreatTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
     	if (bIsLocallyControlled)
     	{
     		SendMouseCursorData();
     	}
     	else
     	{
     		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
     		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
     		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(),GetActivationPredictionKey()).AddUObject(this,&UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
     		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationPredictionKey);
     		if (!bCalledDelegate)
     		{
     			SetWaitingOnRemotePlayerData();
     		}
     	}
	
	
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (PC)
	{
		FHitResult CursorResult;
		PC->GetHitResultUnderCursor(ECC_Target,false,CursorResult);//只有在客户端有效
		FGameplayAbilityTargetDataHandle DataHandle;
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		Data->HitResult = CursorResult;
		DataHandle.Add(Data);
		
		AbilitySystemComponent->ServerSetReplicatedTargetData(
			GetAbilitySpecHandle(),//能力句柄
			GetActivationPredictionKey(),//能力初始预测键
			DataHandle,//句柄
			FGameplayTag(),//标签
			AbilitySystemComponent->ScopedPredictionKey//当前预测键
		);
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ValidData.Broadcast(DataHandle);
		}
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
