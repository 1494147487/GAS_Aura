


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectTypes.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	// 转成子类以访问自定义的 Getter(如 GetStrength)
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	// 确保配置数据资产已被赋值
	check(AttributeInfo);

	//// 从 DataAsset 中查出力量属性对应的显示信息(名字、描述等)
	//FAuraAttributeInfo StrengthInfo = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	//// 补上实际的当前数值
	//StrengthInfo.AttributeValue = AS->GetStrength();
	//// 广播给所有监听的 UI 控件，通知刷新显示
	//AttributeInfoDelegate.Broadcast(StrengthInfo);

	// 遍历「Tag → 属性句柄」表，把每条属性的文案和当前数值广播给菜单 UI
	for (auto& Pair : AS->TagsToAttribute)
	{
		//// Pair.Key 是 GameplayTag，用来在 DataAsset 里查出名字、描述
		//FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		//// Pair.Value 是 FGameplayAttribute 句柄，到这份 AS 上读取实时数值
		//Info.AttributeValue = Pair.Value.GetNumericValue(AS);
		//// 把填好的 Info 发给监听 AttributeInfoDelegate 的控件，刷新对应行
		//AttributeInfoDelegate.Broadcast(Info);
		BroadcastAttributeInfo(Pair.Key, Pair.Value);
	}
	
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// 将 AttributeSet 转换为具体类型，才能访问 TagsToAttribute 这张映射表
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	// 遍历 Tag -> Attribute 映射表，为每一个属性分别绑定变化监听
	for (auto& Pair : AS->TagsToAttribute)
	{
		// 拿到该属性专属的"数值变化"委托，绑定一个 Lambda 回调
		// 注意：Pair 必须按值捕获，否则循环结束后引用悬空
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda(
			[this, Pair, AS](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value);
			}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute)
{
	// 根据 Tag 查出该属性对应的静态展示信息（名字、描述等）
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	// 从 AttributeSet 中取出该属性当前的最新数值
	// （也可以直接用 Data.NewValue，效果相同）
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	// 广播出去，UI 层收到后刷新对应显示
	AttributeInfoDelegate.Broadcast(Info);

}
