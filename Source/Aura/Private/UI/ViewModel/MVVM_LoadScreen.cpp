// Mr.Wang


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
void UMVVM_LoadScreen::InitializeLoadSlotViewModel()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0,LoadSlot_0);
	
	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1,LoadSlot_1);
	
	
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2,LoadSlot_2);
	
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index)
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	AAuraGameModeBase* AuraGameMode =Cast<AAuraGameModeBase>( UGameplayStatics::GetGameMode(this));
	if (!AuraGameMode)
	{
		return;
	}
	LoadSlots[Slot]->SetMapName(AuraGameMode->StartingMapName);
	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SlotStatus = Taken;
	LoadSlots[Slot]->PlayerStart = AuraGameMode->DefaultPlayerStartTag;
	AuraGameMode->SaveSlotData(LoadSlots[Slot],Slot);
	LoadSlots[Slot]->InitializeSlot();
	
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	AuraGameInstance->LoadSlotName = LoadSlots[Slot]->LoadSlotName;
	AuraGameInstance->SlotIndex = LoadSlots[Slot]->SlotIndex;
	AuraGameInstance->PlayerStartTag = AuraGameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for (const TPair<int, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
	CurrentSelectSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(CurrentSelectSlot))
	{
		AAuraGameModeBase::DeleteSlot(CurrentSelectSlot->LoadSlotName,CurrentSelectSlot->SlotIndex);
		CurrentSelectSlot->SlotStatus = Vacant;
		CurrentSelectSlot->InitializeSlot();
		CurrentSelectSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GameMode->GetGameInstance());
	AuraGameInstance->PlayerStartTag = CurrentSelectSlot->PlayerStart;
	AuraGameInstance->SlotIndex = CurrentSelectSlot->SlotIndex;
	AuraGameInstance->LoadSlotName = CurrentSelectSlot->LoadSlotName;
	if (IsValid(CurrentSelectSlot))
	{
		GameMode->TravelToMap(CurrentSelectSlot);
	}
	
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TPair<int, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadScreenSaveGame* LoadScreenSaveGame = GameMode->GetSaveSlotData(LoadSlot.Value->LoadSlotName,LoadSlot.Key);
		
		const FString PlayerName = LoadScreenSaveGame->PlayerName;
		const FString MapName = LoadScreenSaveGame->MapName;
		TEnumAsByte<ESaveSlotStatus> SlotStatus = LoadScreenSaveGame->SaveSlotStatus;
		
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->SlotStatus = SlotStatus;
		LoadSlot.Value->SetPlayerLevel(LoadScreenSaveGame->PlayerLevel);
		LoadSlot.Value->SetMapName(MapName);
		LoadSlot.Value->InitializeSlot();
		LoadSlot.Value->PlayerStart = LoadScreenSaveGame->PlayerStart;
		
	}
}
