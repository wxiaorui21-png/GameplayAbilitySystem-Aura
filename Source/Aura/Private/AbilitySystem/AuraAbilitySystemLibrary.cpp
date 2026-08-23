// Mr.Wang


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Dataflow/DataflowOverlay.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject,FWidgetControllerParams& WidgetParams,AAuraHUD*& OutAuraHUD)
{
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if (OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			WidgetParams.AbilitySystemComponent = ASC;
			WidgetParams.AttributeSet = AS;
			WidgetParams.PlayerController = PC;
			WidgetParams.PlayerState = PS;
			return  true;
		}
	}
	return false;
}

UOverlayaWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,WidgetControllerParams,AuraHUD))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,WidgetControllerParams,AuraHUD))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	AAuraHUD* AuraHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject,WidgetControllerParams,AuraHUD))
	{
		return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttribute(ECharacterClass CharacterClass, float Level,const UObject* WorldContextObject,UAbilitySystemComponent* Asc)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;
	
	AActor* AvatarActor = Asc->GetAvatarActor();
	
	const FCharacterDefaultInfo ClassDefaultInfo = AuraGameMode->CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	
	FGameplayEffectContextHandle PrimaryEffectContext = Asc->MakeEffectContext();
	PrimaryEffectContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimarySpec = Asc->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttribute,Level,PrimaryEffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*PrimarySpec.Data.Get());
	
	FGameplayEffectContextHandle SecondaryEffectContext = Asc->MakeEffectContext();
	SecondaryEffectContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondarySpec = Asc->MakeOutgoingSpec(AuraGameMode->CharacterClassInfo->SecondaryAttribute,Level,SecondaryEffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data.Get());
	
	FGameplayEffectContextHandle VitalEffectContext = Asc->MakeEffectContext();
	VitalEffectContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalSpec = Asc->MakeOutgoingSpec(AuraGameMode->CharacterClassInfo->VitalAttribute,Level,VitalEffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributeFromSave(const UObject* WorldContextObject,
	UAbilitySystemComponent* Asc, ULoadScreenSaveGame* SaveObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;
	AActor* AvatarActor = Asc->GetAvatarActor();
	FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	UCharacterClassInfo* ClassInfo = AuraGameMode->CharacterClassInfo;
	FGameplayEffectContextHandle ContextHandle =  Asc->MakeEffectContext();
	ContextHandle.AddSourceObject(AvatarActor);
	FGameplayEffectSpecHandle SpecHandle = Asc->MakeOutgoingSpec(ClassInfo->PrimaryAttribute_SetByColor,1,ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attributes_Primary_Strength,SaveObject->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attributes_Primary_Intelligence,SaveObject->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attributes_Primary_Resilience,SaveObject->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,GameplayTags.Attributes_Primary_Vigor,SaveObject->Vigor);
	
	Asc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	FGameplayEffectContextHandle SecondaryEffectContext = Asc->MakeEffectContext();
	SecondaryEffectContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondarySpec = Asc->MakeOutgoingSpec(ClassInfo->SecondaryAttribute_Infinite,1,SecondaryEffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*SecondarySpec.Data);
	
	FGameplayEffectContextHandle VitalEffectContext = Asc->MakeEffectContext();
	VitalEffectContext.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalSpec = Asc->MakeOutgoingSpec(ClassInfo->VitalAttribute,1,VitalEffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data);
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* Asc,ECharacterClass CharacterClass)
{
	
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		Asc->GiveAbility(AbilitySpec);
	}
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities = CharacterClassInfo->GetClassDefaultInfo(CharacterClass).InitAbilities;
	for (TSubclassOf<UGameplayAbility> AbilityClass : InitialAbilities)
	{
		
		if (Asc->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,ICombatInterface::Execute_GetPlayerLevel(Asc->GetAvatarActor()));
			Asc->GiveAbility(AbilitySpec);
		}
		
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;
	const FCharacterDefaultInfo Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	const float XPReward = Info.XPReward.GetValueAtLevel(Level);
	
	return static_cast<int32>(XPReward);
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageParams(FDamageEffectParams& DamageEffectParams, bool bIsRadiaDamage,
	float InnerRadius, float OuterRadius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = bIsRadiaDamage;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockBackDirection(FDamageEffectParams& DamageEffectParams,
	FVector KnockBackDirection,float Magnitude)
{
	
	KnockBackDirection.Normalize();
	if (Magnitude == 0)
	{
		DamageEffectParams.KnockImpulseDirection = KnockBackDirection * DamageEffectParams.KnockImpulseMagniture;
	}
	else
	{
		DamageEffectParams.KnockImpulseDirection = KnockBackDirection * Magnitude;
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulseDirection(FDamageEffectParams& DamageEffectParams,
	FVector DeathImpulseDirection,float Magnitude)
{
	DeathImpulseDirection.Normalize();
	if (Magnitude == 0)
	{
		DamageEffectParams.KnockImpulseDirection = DeathImpulseDirection * DamageEffectParams.DeathImpulseMagniture;
	}else
	{
		DamageEffectParams.KnockImpulseDirection = DeathImpulseDirection * Magnitude;
	}
	
}

void UAuraAbilitySystemLibrary::SetAbilitySystemComponent(FDamageEffectParams& DamageEffectParams,
	UAbilitySystemComponent* AbilitySystemComponent)
{
	DamageEffectParams.TargetAbilitySystemComponent = AbilitySystemComponent;
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return nullptr;
	return AuraGameMode->AbilityInfo;
}


bool UAuraAbilitySystemLibrary::IsBlockHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->IsBlockHit();
	}
	return false;
	
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetDebuffDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetDebuffDuration();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetDebuffFrequency();
	}
	return false;
}

FVector UAuraAbilitySystemLibrary::GetImpulseDirection(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetImpulseDirection();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockImpulseDirection(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetKnockImpulseDirection();
	}
	return FVector::ZeroVector;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		if (AuraContext->GetDamageType().IsValid())
		{
			return *AuraContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetIsRadialDamage();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetRadialDamageInnerRadius();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetRadialDamageOuterRadius();
	}
	return false;
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return AuraContext->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetBlockHit(FGameplayEffectContextHandle& ContextHandle, bool bIsBlockHit)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetIsBlockHit(bIsBlockHit);
	}
}

void UAuraAbilitySystemLibrary::SetCriticalHit(FGameplayEffectContextHandle& ContextHandle, bool bIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetIsCriticalHit(bIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetSuccessfulDebuff(FGameplayEffectContextHandle& ContextHandle,
	bool bIsSuccessfulDebuff)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetIsSuccessfulDebuff(bIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& ContextHandle, float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& ContextHandle, float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& ContextHandle, float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& ContextHandle, FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraContext->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetImpulseDirection(FGameplayEffectContextHandle& ContextHandle,
	const FVector& InImpulseDirection)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetImpulseDirection(InImpulseDirection);
	}
}

void UAuraAbilitySystemLibrary::SetKnockImpulseDirection(FGameplayEffectContextHandle& ContextHandle,
	const FVector& InKnockImpulseDirection)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetKnockImpulseDirection(InKnockImpulseDirection);
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& ContextHandle, bool bIsRadiaDamage)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetIsRadialDamage(bIsRadiaDamage);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& ContextHandle,
	float InRadialDamageInnerRadius)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetRadialDamageInnerRadius(InRadialDamageInnerRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& ContextHandle,
	float InRadialDamageOuterRadius)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetRadialDamageOuterRadius(InRadialDamageOuterRadius);
	}
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& ContextHandle,
	const FVector& InRadialDamageOrigin)
{
	if (FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetRadialDamageOrigin(InRadialDamageOrigin);
	}
}

void UAuraAbilitySystemLibrary::GetLiveCharacterWithInRadius(const UObject* WorldContextObject,
                                                             TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorToIgnore, const float Radius,
                                                             const FVector& SphereLocation)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorToIgnore);
	
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps,SphereLocation,FQuat::Identity,FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),FCollisionShape::MakeSphere(Radius),SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			
			if (Overlap.GetActor()->Implements<UCombatInterface>())
			{
				if (!ICombatInterface::Execute_IsDead(Overlap.GetActor()))
					OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetCloseActorsFromLocation(int32 NumsTarget, const TArray<AActor*>& OverlappingActors,TArray<AActor*>& OutAdditionalActors, const FVector& Location)
{
	if (OverlappingActors.Num() <= NumsTarget)
	{
		OutAdditionalActors = OverlappingActors;
		return;
	}
	TArray<AActor*> CheckToActors = OverlappingActors;
	int NumsActor = 0;
	while (NumsActor < NumsTarget)
	{
		if (CheckToActors.Num() == 0) return;
		double MinDistance = TNumericLimits<double>::Max();
		AActor* ClosedActor;
		for (AActor* Actor : CheckToActors)
		{
			double Distance = (Actor->GetActorLocation()-Location).Length();
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosedActor = Actor;
			}
		}
		CheckToActors.Remove(ClosedActor);
		OutAdditionalActors.AddUnique(ClosedActor);
		NumsActor++;
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle ContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor());
	FGameplayEffectSpecHandle DamageEffectSpec = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass,DamageEffectParams.AbilityLevel,ContextHandle);
	
	SetImpulseDirection(ContextHandle,DamageEffectParams.ImpulseDirection);
	SetKnockImpulseDirection(ContextHandle,DamageEffectParams.KnockImpulseDirection);
	SetIsRadialDamage(ContextHandle,DamageEffectParams.bIsRadialDamage);
	SetRadialDamageOuterRadius(ContextHandle,DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageInnerRadius(ContextHandle,DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOrigin(ContextHandle,DamageEffectParams.RadialDamageOrigin);
	
	//将标签赋予值之后赋予给Spec，以便域之后进行计算之类。
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec,DamageEffectParams.DamageType,DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec,GameplayTags.Debuff_Duration,DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec,GameplayTags.Debuff_Damage,DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec,GameplayTags.Debuff_Frequency,DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpec,GameplayTags.Debuff_Chance,DamageEffectParams.DebuffChance);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageEffectSpec.Data);
	//后续可能会使用到什么就返回什么
	return ContextHandle; 
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpaceRotator(const FVector& Forward, const FVector& Axis,float SpreadAngle, int32 NumsRotator)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpreadAngle/2,FVector::UpVector);
	if (NumsRotator > 1)
	{
		const float Spread = SpreadAngle/(NumsRotator - 1);
		for (int32 i=0;i<NumsRotator;i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(Spread * i,FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}else
	{
		Rotators.Add(Forward.Rotation());
	}
	return Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotationVector(const FVector& Forward, const FVector& Axis,
	float SpreadAngle, int32 NumsVector)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpreadAngle/2,FVector::UpVector);
	if (NumsVector > 1)
	{
		const float Spread = SpreadAngle/(NumsVector - 1);
		for (int32 i=0;i<NumsVector;i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(Spread * i,FVector::UpVector);
			Vectors.Add(Direction);
		}
	}else
	{
		Vectors.Add(Forward);
	}
	return Vectors;
}


