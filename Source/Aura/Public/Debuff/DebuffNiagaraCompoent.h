// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraCompoent.generated.h"


/**
 * 
 */
UCLASS()
class AURA_API UDebuffNiagaraCompoent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	UDebuffNiagaraCompoent();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DebuffTag;
	
	void DebuffChangedEvent(const FGameplayTag Tag,int32 NewCount);
	
	UFUNCTION()
	void CharacterDeath(AActor* Character);
};
