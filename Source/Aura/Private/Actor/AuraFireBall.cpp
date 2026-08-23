// Mr.Wang


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayCueManager.h"
#include "Components/AudioComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void AAuraFireBall::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters GameplayCueParameters;
		GameplayCueParameters.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(),FAuraGameplayTags::Get().GameplayCue_FireBlast,GameplayCueParameters);
	}
	if (LoopSoundComponent)
	{
		LoopSoundComponent->Stop();
		LoopSoundComponent->DestroyComponent();
	}
	
	bHit = true;
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector ImpulseDir = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagniture;
			
			DamageEffectParams.ImpulseDirection = ImpulseDir;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
