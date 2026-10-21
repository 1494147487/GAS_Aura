


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP)const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		// 数组下标 = 等级：[1] 是 1 级的信息，[2] 是 2 级的信息
		if (LevelUpInformation.Num() - 1 <= Level) return Level;   // 已到最高级，没有下一级可升，直接返回

		if (XP >= LevelUpInformation[Level].LevelUpRequirement)    // 经验够升到下一级
		{
			++Level;                                               // 等级加一，继续判断
		}
		else
		{
			bSearching = false;                                    // 经验不够，停止
		}
	}
	return Level;
}
