

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;   // 升到这一级所需的累计经验

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;  // 升到这一级奖励的属性点

	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;      // 升到这一级奖励的技能点
};


UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraLevelUpInfo> LevelUpInformation;

	int32 FindLevelForXP(int32 XP)const;
};
