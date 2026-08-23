// Mr.Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULoadScreenSaveGame;
class UMVVM_LoadSlot;
class USaveGame;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	
	UPROPERTY(EditDefaultsOnly,Category = "Character Class Defaults")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	static void DeleteSlot(const FString& LoadSlotName, int32 SlotIndex);
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot,int32 SlotIndex);
	void SaveInGameData(ULoadScreenSaveGame* SaveObject);
	
	void SaveWorldState(UWorld* World);
	void LoadWorldState(UWorld* World);
	
	void TravelToMap(UMVVM_LoadSlot* LoadSlot);
	
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName,int32 SlotIndex);
	ULoadScreenSaveGame* RetrieveInGameData();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
	
	UPROPERTY(EditDefaultsOnly)
	FString StartingMapName;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;
	
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FString,TSoftObjectPtr<UWorld>> Maps;
	
	
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
protected:
	virtual void BeginPlay() override;
};
