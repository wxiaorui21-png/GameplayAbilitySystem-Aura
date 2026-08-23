// Mr.Wang


#include "CheckPoint/CheckPoint.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

ACheckPoint::ACheckPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)//需要先调用父类的构造函数
{
	PrimaryActorTick.bCanEverTick = false;
	
	PointMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("PointMesh"));
	PointMesh->SetupAttachment(GetRootComponent());
	PointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	
	Sphere = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	Sphere->SetupAttachment(PointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	
	
	
}

void ACheckPoint::LoadActor_Implementation()
{
	if (bReached)
	{
		HandleGlowEffects();
	}
	
}

void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&ACheckPoint::OnSphereOverlap);
}

void ACheckPoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())//不需要再判断OtherActor中是否具有角色标签
	{
		bReached = true;
		if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			AuraGameModeBase->SaveWorldState(GetWorld());
		}
		IPlayerInterface::Execute_SavePlayerInGameData(OtherActor,PlayerStartTag);
		HandleGlowEffects();
	}
}

void ACheckPoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Creat
	UMaterialInstanceDynamic* DynamicInstance = UMaterialInstanceDynamic::Create(PointMesh->GetMaterial(0),this);
	//Give
	PointMesh->SetMaterial(0, DynamicInstance);
	CheckPointReached(DynamicInstance);
}
