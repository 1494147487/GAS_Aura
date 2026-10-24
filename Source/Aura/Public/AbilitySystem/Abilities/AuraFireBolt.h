

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectSpell
{
	GENERATED_BODY()

public:

	virtual FString GetDescription(int32 Level)override;
	virtual FString GetNextLevelDescription(int32 Level)override;
};
