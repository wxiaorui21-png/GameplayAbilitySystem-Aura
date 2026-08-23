// Mr.Wang


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP)
{
	int Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		//LevelUpInfo[1] = level 1
		//LevelUpInfo[2] = level 2
		//LevelUpInfo[0]设置占位符
		if (LevelUpInfo.Num() - 1 <= Level) return Level;
		
		if (XP >= LevelUpInfo[Level].LevelUpRequirement)
		{
			Level++;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
