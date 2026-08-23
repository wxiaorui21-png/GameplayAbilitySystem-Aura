// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveGameInterface.h"
#include "CheckPoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class AURA_API ACheckPoint : public APlayerStart,public ISaveGameInterface
{
	GENERATED_BODY()
public:
	ACheckPoint(const FObjectInitializer& ObjectInitializer);//跟随父类PlayerStart的构成函数
	
	UPROPERTY(BlueprintReadOnly,SaveGame)//SaveGame用来使其序列化，变化是保存
	bool bReached = false;
	
	/* SaveGame Interface*/
	virtual bool ShouldLoadTransform_Implementation() override {return false;}
	virtual void LoadActor_Implementation() override;
	
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
	
	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached(UMaterialInstanceDynamic* DynamicInstanceCheckPoint);//调解动态材质参数使其发光，由于需要使用到时间轴，因此使用蓝图实现
	
	void HandleGlowEffects();//创建动态材质，并且使其发光
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PointMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	
	
};
