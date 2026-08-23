// Mr.Wang


#include "Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseData(const FHitResult& MouseHit)
{
	if (MouseHit.bBlockingHit == true)
	{
		MouseHitLocation = MouseHit.ImpactPoint;
		MouseHitActor = MouseHit.GetActor();
	}else
	{
		CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
	}
}

void UAuraBeamSpell::StoreOwningVariable()
{
	if (CurrentActorInfo)
	{
		PlayerController = CurrentActorInfo->PlayerController.Get();
	}
	if (CurrentActorInfo)
	{
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamSpellTarget)
{
	check(OwnerCharacter)
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeaponComponent(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			const FVector WeaponSocket = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				WeaponSocket,
				BeamSpellTarget,
				10.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true);
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeath().IsAlreadyBound(this,&UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeath().AddDynamic(this,&UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTarget(TArray<AActor*>& OnOutAdditionalTargets)
{  
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLiveCharacterWithInRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation());
	
	int32 NumsAdditionalActors = 5;
	UAuraAbilitySystemLibrary::GetCloseActorsFromLocation(NumsAdditionalActors,OverlappingActors,OnOutAdditionalTargets,MouseHitActor->GetActorLocation());

	for (AActor* Actor : OverlappingActors)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
		{
			if (!CombatInterface->GetOnDeath().IsAlreadyBound(this,&UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeath().AddDynamic(this,&UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}
