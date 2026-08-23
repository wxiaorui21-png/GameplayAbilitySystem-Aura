// Mr.Wang


#include "Debuff/DebuffNiagaraCompoent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraCompoent::UDebuffNiagaraCompoent()
{
	bAutoActivate = false;
	
}

void UDebuffNiagaraCompoent::BeginPlay()
{
	Super::BeginPlay();
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UDebuffNiagaraCompoent::DebuffChangedEvent);
	}else if (CombatInterface)
	{
		CombatInterface->GetOnDebuffGetASC().AddLambda(
			[this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&UDebuffNiagaraCompoent::DebuffChangedEvent);
		});
	}
	if (CombatInterface)
	{
		CombatInterface->GetOnDeath().AddDynamic(this,&UDebuffNiagaraCompoent::CharacterDeath);
	}
	
}

void UDebuffNiagaraCompoent::DebuffChangedEvent(const FGameplayTag Tag,int32 NewCount)
{
	bool bOnValid = IsValid(GetOwner());
	bool bOnActive = GetOwner()->Implements<UCombatInterface>()&& !ICombatInterface::Execute_IsDead(GetOwner());
	
	if (NewCount > 0 && bOnValid && bOnActive)
	{
		Activate();
	}else
	{
		Deactivate();
	}
}

void UDebuffNiagaraCompoent::CharacterDeath(AActor* Character)
{
	Deactivate();
}
