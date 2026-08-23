// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

/**
 * 
 */

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};
USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "ClassDefaults")
	FGameplayTag AbilityTag = FGameplayTag(); 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "ClassDefaults")
	FGameplayTag AbilityStatus = FGameplayTag(); 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "ClassDefaults")
	FGameplayTag AbilitySlot = FGameplayTag(); 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "ClassDefaults")
	FGameplayTag AbilityType = FGameplayTag(); 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category= "ClassDefaults")
	int32 AbilityLevel;
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag == Right.AbilityTag;
}



USTRUCT()
struct FSavedActor//用来保存世中需要保存的对象，例如检查点，为什么不用数组的Actor*保存，因为指针其实是内存地址，重写编译时不一定会使其地址相同。
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ActorName = FName();//用来标识唯一的保存对象
	
	UPROPERTY()
	FTransform Transform = FTransform();
	
	//同来确定对象中哪些属性序列化了，即使用过，其中只包含SaveGame保存的属性
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT()//用来确保一个地推对应他们自己所要保存的角色类
struct FSavedMap
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString MapAssetName = FString();
	
	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};


UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;
	
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	
	UPROPERTY()
	FString MapName = FString("Default Map Name");
	
	UPROPERTY()
	FName PlayerStart;
	
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;
	
	UPROPERTY()
	bool bFirstLoad = true;
	
	/*Player*/
	UPROPERTY()
	int32 PlayerLevel = 1;
	
	UPROPERTY()
	int32 XP = 0;
	
	UPROPERTY()
	int32 SpellPoints = 0;
	
	UPROPERTY()
	int32 AttributePoints = 0;
	
	//由于该项目的次要属性和关键属性都来源与主要属性，因此只需要保存主要属性
	
	/*Attributes*/
	UPROPERTY()
	float Strength = 0;
	
	UPROPERTY()
	float Intelligence = 0;
	
	UPROPERTY()
	float Resilience = 0;
	
	UPROPERTY()
	float Vigor = 0;
	
	/*Abilities*/
	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;
	
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;
	
	//用来获取Map
	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	
	//用来判断是否返回的是有效的映射
	bool HasMap(const FString& InMapName);
	
};
