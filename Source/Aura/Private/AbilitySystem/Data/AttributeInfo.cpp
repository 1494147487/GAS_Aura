


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeInfomation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}

	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("在 AttributeInfo [%s] 中找不到 AttributeTag [%s] 对应的属性信息。"), *GetNameSafe(this), *AttributeTag.ToString());
	}

	return FAuraAttributeInfo();

}
