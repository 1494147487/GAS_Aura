

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	// 标识这条数据对应哪一个属性(比如力量、护甲)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	// 属性在 UI 上显示的名字
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	// 属性的描述文字(比如 Tooltip 说明)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();

	// 属性当前的数值，运行时赋值，不在编辑器里配置
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};

UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
	

public:

	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag,bool bLogNotFound = false)const;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInfomation;
};
