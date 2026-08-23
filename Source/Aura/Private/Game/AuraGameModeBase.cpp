// Mr.Wang


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveGameInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	//return nullptr;返回空会出现在默认位置，同时也不会生成角色
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APlayerStart::StaticClass(),PlayerActors);
	if (PlayerActors.Num()>0)
	{
		AActor* SelectActor = PlayerActors[0];//没有对应标签则返回第一个，一般来说是PIE（编辑器运行时）UE自动生成的一个APlayerStartPIE
		for (AActor* Actor : PlayerActors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectActor =  PlayerStart;//选择带有特定标签的返回
				}
			}
		}
		return SelectActor;
	}
	return nullptr;//没有则返回空
	
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(StartingMapName,DefaultMap);
}

void AAuraGameModeBase::DeleteSlot(const FString& LoadSlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlotName, SlotIndex);
	}
}

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	DeleteSlot(LoadSlot->LoadSlotName, SlotIndex);
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->PlayerStart = LoadSlot->PlayerStart;
	
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame,LoadSlot->LoadSlotName, SlotIndex);
}

void AAuraGameModeBase::SaveInGameData(ULoadScreenSaveGame* SaveObject)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString SlotName = AuraGameInstance->LoadSlotName;
	const int32 SlotIndex = AuraGameInstance->SlotIndex;
	AuraGameInstance->PlayerStartTag = SaveObject->PlayerStart;
	
	UGameplayStatics::SaveGameToSlot(SaveObject,SlotName, SlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World)
{
	FString WorldName = World->GetMapName();//由于得到的地图包含UE生成的前缀，因此需要去除掉前缀
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	
	//确定保存在哪个插槽中
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);
	
	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGameInstance->LoadSlotName,AuraGameInstance->SlotIndex))
	{
		//确保SaveGame中的有该地图的FSaveMap
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}
		//如何判断这个对象是否需要保存，可以实现一个接口
		
		//得到我们需要保存的Map对象
		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		//提前置为空
		SavedMap.SavedActors.Empty();
		//循环中各关卡中的对象
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			//实现接口的说明是需要保存的对象
			if (!IsValid(Actor)||!Actor->Implements<USaveGameInterface>()) continue;
			
			//创建需要保存的对象
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();
			
			//Actor 状态序列化为字节
			FMemoryWriter MemoryWriter(SavedActor.Bytes);//MemoryWriter 把后续写入的数据放进 SavedActor.Bytes（TArray<uint8>）。
			//代理 Archive 会将 UObject*、FName 等以字符串等可持久化形式处理；
			//ArIsSaveGame = true 表示这是 SaveGame 序列化，只处理带有 UPROPERTY(SaveGame) 标记的属性。
			//Actor->Serialize 随即把这些属性写进字节数组。
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter,true);
			Archive.ArIsSaveGame = true;
			Actor->Serialize(Archive);
			
			SavedMap.SavedActors.Add(SavedActor);
		}
		//更新插槽中要保存的数据
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame,AuraGameInstance->LoadSlotName,AuraGameInstance->SlotIndex);
	}
	
}

void AAuraGameModeBase::LoadWorldState(UWorld* World)
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGameInstance);
	
	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName,AuraGameInstance->SlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGameInstance->LoadSlotName,AuraGameInstance->SlotIndex));
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura,Error,TEXT("Failed to Load Slot"));
			return;
		}
		//加载数据
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->Implements<USaveGameInterface>()) continue;
			//更新保存对象
			for (FSavedActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveGameInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}
					//这次 Archive 是“读”模式，因此相同的 Serialize 调用会把字节中的值写回 Actor 属性。
					//方向不由 Serialize 参数决定，而是由底层使用 FMemoryWriter 还是 FMemoryReader 决定。
					FMemoryReader MemoryReader(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader,true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);
					
					ISaveGameInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
	
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* LoadSlot)
{
	//传入完整的ViewModel的作用：可以获取到其中所以的信息，可以看情况是否使用
	const FString SlotName = LoadSlot->LoadSlotName;
	const int32 SlotIndex = LoadSlot->SlotIndex;
	
	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot,Maps.FindChecked(LoadSlot->GetMapName()));
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex)
{
	ULoadScreenSaveGame* LoadScreenSaveGame = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName,SlotIndex))
	{
		LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName,SlotIndex));
	}else
	{
		LoadScreenSaveGame =Cast<ULoadScreenSaveGame>( UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
	}
	return LoadScreenSaveGame;

}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameData()
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString SlotName = AuraGameInstance->LoadSlotName;
	const int32 SlotIndex = AuraGameInstance->SlotIndex;
	
	return GetSaveSlotData(SlotName,SlotIndex);
}
