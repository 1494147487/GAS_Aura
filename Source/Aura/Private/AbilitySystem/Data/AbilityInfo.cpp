


#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bNotLogFound) const
{
	for (const FAuraAbilityInfo& Info : AbilityInformation)
	{
		if(Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	return FAuraAbilityInfo();
}
