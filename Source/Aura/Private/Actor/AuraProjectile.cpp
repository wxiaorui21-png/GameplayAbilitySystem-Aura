// Mr.Wang


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"


AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;//使其在客户端只是服务端的一个复制。
	
	
	Sphere = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}



void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraProjectile::OnSphereOverlap);
	
	LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopSound,GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
	if (bHit&&!HasAuthority())//防止服务器端再一次播放
	{
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
		bHit = false;
		if (LoopSoundComponent)
		{
			LoopSoundComponent->Stop();
			LoopSoundComponent->DestroyComponent();
		}
	}
	Super::Destroyed();
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
	if (LoopSoundComponent)
	{
		LoopSoundComponent->Stop();
		LoopSoundComponent->DestroyComponent();
	}
	bHit = true;
}

bool AAuraProjectile::IsValidOverlap(AActor* OtherActor) const
{
	if(!DamageEffectParams.SourceAbilitySystemComponent) return false;
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor)
	{
		return false;
	}
	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor,OtherActor))
	{
		return false;
	}
	return true;
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;
	if (!HasAuthority())
	{
		bHit = true;
	}
	
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
	if (LoopSoundComponent)
	{
		LoopSoundComponent->Stop();
		LoopSoundComponent->DestroyComponent();
	}
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector ImpulseDir = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagniture;
			bool bIsKnock = FMath::FRandRange(0.f,100.f) < DamageEffectParams.KnockChance ? true : false;
			if (bIsKnock)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				FVector Dir = Rotation.Vector();
				const FVector KnockImpulseDir = Dir* DamageEffectParams.KnockImpulseMagniture;
				DamageEffectParams.KnockImpulseDirection = KnockImpulseDir;
			}else
			{
				DamageEffectParams.KnockImpulseDirection = FVector::ZeroVector;
			}
			DamageEffectParams.ImpulseDirection = ImpulseDir;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		
		Destroy();
	}
	
}


