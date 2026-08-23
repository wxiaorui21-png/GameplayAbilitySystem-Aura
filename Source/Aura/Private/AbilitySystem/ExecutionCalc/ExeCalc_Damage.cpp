// Mr.Wang


#include "AbilitySystem/ExecutionCalc/ExeCalc_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"


struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,Armor,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArmorPenetration,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,BlockChance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitChance,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitDamage,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,CriticalHitResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,FireResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,LightningResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,PhysicalResistance,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet,ArcaneResistance,Target,false);
		
	}
};

static  const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatic;
	return DStatic;
}


UExeCalc_Damage::UExeCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExeCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                             FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag,FGameplayEffectAttributeCaptureDefinition> TagsToCapture;
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
	TagsToCapture.Add(Tags.Attributes_Secondary_Armor,DamageStatics().ArmorDef);
	TagsToCapture.Add(Tags.Attributes_Secondary_ArmorPenetration,DamageStatics().ArmorPenetrationDef);
	TagsToCapture.Add(Tags.Attributes_Secondary_BlockChance,DamageStatics().BlockChanceDef);
	TagsToCapture.Add(Tags.Attributes_Secondary_CriticalHitChance,DamageStatics().CriticalHitChanceDef);
	TagsToCapture.Add(Tags.Attributes_Secondary_CriticalHitDamage,DamageStatics().CriticalHitDamageDef);
	TagsToCapture.Add(Tags.Attributes_Secondary_CriticalHitResistance,DamageStatics().CriticalHitResistanceDef);
		
	TagsToCapture.Add(Tags.Attributes_Resistance_Fire,DamageStatics().FireResistanceDef);
	TagsToCapture.Add(Tags.Attributes_Resistance_Lightning,DamageStatics().LightningResistanceDef);
	TagsToCapture.Add(Tags.Attributes_Resistance_Physical,DamageStatics().PhysicalResistanceDef);
	TagsToCapture.Add(Tags.Attributes_Resistance_Arcane,DamageStatics().ArcaneResistanceDef);
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);		
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);		
	}
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	//debuff
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	for (TPair<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuff)
	{
		FGameplayTag DamageType= Pair.Key;
		FGameplayTag DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType,false,-1.f);
		if (TypeDamage > -.5f)//将精度范围设置为0.5
		{
			FGameplayTag ResistanceType = GameplayTags.DamageTypesToResistance[DamageType];
			float TargetDebuffResistanceType = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagsToCapture[ResistanceType],EvaluateParameters,TargetDebuffResistanceType);
			TargetDebuffResistanceType = FMath::Max<float>(TargetDebuffResistanceType,0.f);
			const float DebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance,false,-1.f);
			const float DebuffChanceEffective = DebuffChance*(100 - TargetDebuffResistanceType)/100;
			const bool bDebuff = FMath::RandRange(0,100) < DebuffChanceEffective;
			if (bDebuff)
			{
				FGameplayEffectContextHandle SpecHandle = Spec.GetContext();
				UAuraAbilitySystemLibrary::SetSuccessfulDebuff(SpecHandle,true);
				
				float Damage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage,false,-1.f);
				float Duration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration,false,-1.f);
				float Frequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency,false,-1.f);
				
				UAuraAbilitySystemLibrary::SetDebuffDamage(SpecHandle,Damage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(SpecHandle,Duration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(SpecHandle,Frequency);
				UAuraAbilitySystemLibrary::SetDamageType(SpecHandle,DamageType);
			}
		}
	}
	
	//伤害由调用者的幅度设定
	float Damage = 0;
	for (const TPair<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistance)
	{
		
		
		const FGameplayTag DamageType = Pair.Key;
		const FGameplayTag Resistance = Pair.Value;
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCapture[Resistance];
		
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageType,false);//第三个参数默认没找到为0，因此不需要额外设置没找到的结果
		
		if (DamageTypeValue <= 0.f) continue;
		
		
		float ResistanceValue = 0;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef,EvaluateParameters,ResistanceValue);
		ResistanceValue = FMath::Clamp(ResistanceValue,0.f,100.f);
		
		DamageTypeValue *= (100-ResistanceValue)/100.f;
		if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
			{
				CombatInterface->GetOnDamageSingure().AddLambda([&](float RadialDamage)
				{
					DamageTypeValue = RadialDamage;
				});
			}
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvatar,
				DamageTypeValue,
				0,
				UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				SourceAvatar,
				nullptr
			);
		}
		
		
		
		
		Damage += DamageTypeValue;
	}
	
	//捕获目标格挡几率，判断是否格挡成功
	//如果格挡成功，则伤害变为一半。
	float BlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef,EvaluateParameters,BlockChance);
	BlockChance = FMath::Max(BlockChance,0.f);
	
	const float RandValue = FMath::FRandRange(0.f,100.f);
	bool IsBlock = RandValue <= BlockChance;
	Damage = IsBlock ? Damage/2.f : Damage;
	
	UAuraAbilitySystemLibrary::SetBlockHit(EffectContextHandle,IsBlock);
	
	//护甲穿透忽略目标护甲的百分比
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef,EvaluateParameters,TargetArmor);
	TargetArmor = FMath::Max(TargetArmor,0.f);
	
	float TargetArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef,EvaluateParameters,TargetArmorPenetration);
	TargetArmorPenetration = FMath::Max(TargetArmorPenetration,0.f);
	
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* RealCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("ArmorPenetration"),FString());
	const float ArmorPenetrationCoefficient = RealCurve->Eval(SourcePlayerLevel);
	
	
	
	const float EffectiveArmor = TargetArmor *= (100 - TargetArmorPenetration * ArmorPenetrationCoefficient)/100.f;
	
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("EffectiveArmor"),FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);
	
	Damage *= (100-EffectiveArmor * EffectiveArmorCoefficient)/100.f;
	
	//加入暴击以及暴击伤害加成和暴击抗性
	float SourceCriticalHitChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef,EvaluateParameters,SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(SourceCriticalHitChance,0.f);
	
	float SourceCriticalHitDamage = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef,EvaluateParameters,SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(SourceCriticalHitDamage,0.f);
		
	float TargetCriticalHitResistance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef,EvaluateParameters,TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance,0.f);
	bool IsCriticalHitChance = FMath::FRandRange(0.f,100.f)<=SourceCriticalHitChance;
	if (IsCriticalHitChance)
	{
		const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficient->FindCurve(FName("CriticalHitResistance"),FString());
		const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);
		const float EffectiveCriticalDamage = SourceCriticalHitDamage *= (100 - TargetCriticalHitResistance)/100.f;
		Damage *= (100 + EffectiveCriticalDamage * CriticalHitResistanceCoefficient)/100.f;
	}
	UAuraAbilitySystemLibrary::SetCriticalHit(EffectContextHandle,IsCriticalHitChance);
	
	
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive,Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
