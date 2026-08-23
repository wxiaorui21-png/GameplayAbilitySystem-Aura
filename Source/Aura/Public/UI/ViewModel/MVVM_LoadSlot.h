// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);
/**
 *
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;
	
	void InitializeSlot();
	
	
	UPROPERTY()
	FString LoadSlotName;
	
	/*
	 * 该项目使用的是通过检查点作为角色的出生地点，到达不同的检查点是保存为初始点，因此保存的应该是标签名字用来选择初始点
	 * 其他方法：也可以将角色的位置进行保存写入和读取磁盘，得到角色上次所在位置而不是检查点
	 */
	UPROPERTY()
	FName PlayerStart;
	
	UPROPERTY()
	int32 SlotIndex;
	
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;
	
	
	void SetPlayerName(const FString& InPlayer);
	
	FString GetPlayerName() const { return PlayerName;}
	
	void SetMapName(const FString& InMap);
	
	FString GetMapName() const { return MapName;}
	
	void SetPlayerLevel(const int32 InPlayerLevel);
	
	int32 GetPlayerLevel() const { return PlayerLevel;}
private:
	/*Field Notifies*/
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta = (AllowPrivateAccess = "true"))
	FString PlayerName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta = (AllowPrivateAccess = "true"))
	FString MapName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta = (AllowPrivateAccess = "true"))
	int32 PlayerLevel;
};
