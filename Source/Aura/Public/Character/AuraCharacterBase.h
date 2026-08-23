// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraCharacterBase.generated.h"

class UPassiveNiagaraComponent;
class UDebuffNiagaraCompoent;
class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;
class UAnimMontage;
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter,public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{return AttributeSet;};
	/*Combat Interface*/
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual void Die(const FVector& ImpulseDir) override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void AddMinionsCount_Implementation(int32 AddCount) override;
	virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnDebuffGetASC& GetOnDebuffGetASC() override;
	virtual FOnDeath& GetOnDeath() override;
	virtual FOnDamageSingture& GetOnDamageSingure() override;
	virtual USkeletalMeshComponent* GetWeaponComponent_Implementation() override;
	virtual void SetInBeingShockLoop_Implementation(bool InShockLoop) override;
	virtual bool GetBeingShockLoop_Implementation() override;
	/*End Combat Interface*/
	
	FOnDebuffGetASC DebuffGetASCDelegate;
	FOnDeath DeathDelegate;
	FOnDamageSingture DamageDelegate;
	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticastHandleDeath(const FVector& ImpulseDir);
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	UPROPERTY(ReplicatedUsing = OnRep_Stunned,BlueprintReadOnly)
	bool bIsStunned = false;
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool bBeingShockLoop = false;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnRep_Stunned();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	float BaseWalkSpeed = 600.f;
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	FName WeaponTipSocketName;//使用来施法的释放技能所需要的位置。
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	FName RightHandTipSocketName;//使用来施法的释放技能所需要的位置。
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	FName LeftHandTipSocketName;//使用来施法的释放技能所需要的位置。
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	FName TailTipSocketName;//使用来施法的释放技能所需要的位置。
	
	bool bDead = false;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	virtual void InitAbilityActorInfo();
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass,const float Level) const;
	virtual void InitializeDefaultAttributes() const;
	
	void AddCharacterAbilities();
	
	virtual void StunnedTagChanged(const FGameplayTag CallbackTag,int32 NewCount);
	
	/* Dissolve Effects*/
	
	void Dissolve();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	TObjectPtr<USoundBase> DeathSound;
	
	/* MinionCount*/
	
	int32 MinionCount = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	TObjectPtr<UDebuffNiagaraCompoent> BurnNiagaraCompoent;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Character Class Defaults")
	TObjectPtr<UDebuffNiagaraCompoent> StunNiagaraCompoent;
private:
	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbilities;
	
	UPROPERTY(EditAnywhere,Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProjectionNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;
	
	//目的是将Niagara统一附着在这个上面，统一管理，例如让其不随着角色移动
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachment;
	
};
