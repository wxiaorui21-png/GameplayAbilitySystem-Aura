// Mr.Wang


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
