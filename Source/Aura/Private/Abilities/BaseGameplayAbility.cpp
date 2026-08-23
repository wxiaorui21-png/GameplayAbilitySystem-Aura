// Mr.Wang


#include "Abilities/BaseGameplayAbility.h"

FString UBaseGameplayAbility::GetSpellDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, Level : %d</>,"),TEXT("dlajdfkajdlffda"),Level);
}

FString UBaseGameplayAbility::GetSpellNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, Level : %d</>,"),TEXT("Next"),Level);
}

FString UBaseGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until: %d.</>"),Level);
}
