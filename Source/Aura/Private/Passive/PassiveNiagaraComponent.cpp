// Mr.Wang


#include "Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::ActiveIfEquipped(UAuraAbilitySystemComponent* ASC)
{
	const bool bStartupAbilitiesGiven = ASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (ASC->GetStatusTagFromTag(PassiveTag) == FAuraGameplayTags::Get().Ability_Status_Equipped)
		{
			Activate();
		}
	}
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		ASC->PassiveNiagaraComponent.AddUObject(this,&UPassiveNiagaraComponent::PassiveActivate);
		ActiveIfEquipped(ASC);//目的是防止还没有绑定，ASC已经发送了广播，导致错过了广播，因此需要主动激活（已装备的）
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnDebuffGetASC().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->PassiveNiagaraComponent.AddUObject(this,&UPassiveNiagaraComponent::PassiveActivate);
				ActiveIfEquipped(AuraASC);
			}
			
		});
	}
	
}

void UPassiveNiagaraComponent::PassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveTag))
	{
		if (bActivate&&!IsActive())
		{
			Activate();
		}else
		{
			Deactivate();
		}
	}
}
