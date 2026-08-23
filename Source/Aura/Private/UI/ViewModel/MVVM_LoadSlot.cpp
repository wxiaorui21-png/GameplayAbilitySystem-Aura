// Mr.Wang


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	const int32 Status = SlotStatus.GetIntValue();
	SetWidgetSwitcherIndex.Broadcast(Status);
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayer)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName,InPlayer);
}

void UMVVM_LoadSlot::SetMapName(const FString& InMap)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName,InMap);
}

void UMVVM_LoadSlot::SetPlayerLevel(const int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel,InPlayerLevel);
}
