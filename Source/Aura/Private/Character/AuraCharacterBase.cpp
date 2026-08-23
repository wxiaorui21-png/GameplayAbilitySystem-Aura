// Mr.Wang


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Debuff/DebuffNiagaraCompoent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Passive/PassiveNiagaraComponent.h"


AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BurnNiagaraCompoent = CreateDefaultSubobject<UDebuffNiagaraCompoent>("BurnNiagaraComponent");
	BurnNiagaraCompoent->SetupAttachment(GetRootComponent());
	BurnNiagaraCompoent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
	
	StunNiagaraCompoent = CreateDefaultSubobject<UDebuffNiagaraCompoent>("StunNiagaraComponent");
	StunNiagaraCompoent->SetupAttachment(GetRootComponent());
	StunNiagaraCompoent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	EffectAttachment = CreateDefaultSubobject<USceneComponent>("EffectAttachment");
	EffectAttachment->SetupAttachment(GetRootComponent());
	HaloOfProjectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProjectionNiagaraComponent");
	HaloOfProjectionNiagaraComponent->SetupAttachment(EffectAttachment);
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonNiagaraComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachment);
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonNiagaraComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachment);
}

void AAuraCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EffectAttachment->SetWorldRotation(FRotator::ZeroRotator);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase,bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase,bBeingShockLoop);
}

float AAuraCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageDelegate.Broadcast(Damage);
	return Damage;
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& ImpulseDir)
{
	
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	MulticastHandleDeath(ImpulseDir);
	
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& ImpulseDir)
{
	UGameplayStatics::PlaySoundAtLocation(this,DeathSound,GetActorLocation(),GetActorRotation());
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(ImpulseDir*0.1,NAME_None,true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	GetMesh()->AddImpulse(ImpulseDir,NAME_None,true);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Dissolve();
	bDead = true;//不需要设置同步的原因是多播可以在客户端和服务端同时设置。
	
	DeathDelegate.Broadcast(this);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AAuraCharacterBase::OnRep_Stunned()
{
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	//根据动画的标签传入相对应的插槽
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.Combat_Socket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailTipSocketName);
	}
	
	return FVector();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::AddMinionsCount_Implementation(int32 AddCount)
{
	MinionCount += AddCount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnDebuffGetASC& AAuraCharacterBase::GetOnDebuffGetASC()
{
	return DebuffGetASCDelegate;
}

FOnDeath& AAuraCharacterBase::GetOnDeath()
{
	return DeathDelegate;
}

FOnDamageSingture& AAuraCharacterBase::GetOnDamageSingure()
{
	return DamageDelegate;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeaponComponent_Implementation()
{
	return Weapon;
}

void AAuraCharacterBase::SetInBeingShockLoop_Implementation(bool InShockLoop)
{
	bBeingShockLoop = InShockLoop;
}

bool AAuraCharacterBase::GetBeingShockLoop_Implementation()
{
	return bBeingShockLoop;
}


void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffectClass, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle GameplayEffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	GameplayEffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Level,GameplayEffectContextHandle);
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*GameplayEffectSpec.Data.Get(),GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes,1);
	ApplyEffectToSelf(DefaultSecondaryAttributes,1);
	ApplyEffectToSelf(DefaultVitalAttributes,1);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;
	
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::StunnedTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance,this);
		GetMesh()->SetMaterial(0,DynamicMatInst);
		StartDissolveTimeLine(DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance,this);
		Weapon->SetMaterial(0,DynamicMatInst);
		StartWeaponDissolveTimeLine(DynamicMatInst);
	}
}




