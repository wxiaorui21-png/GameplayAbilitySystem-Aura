// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "PassiveNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UPassiveNiagaraComponent();
	void ActiveIfEquipped(UAuraAbilitySystemComponent* ASC);

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveTag;
	
protected:
	virtual void BeginPlay() override;
	void PassiveActivate(const FGameplayTag& AbilityTag,bool bActivate);
};
