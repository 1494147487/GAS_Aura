


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include <Interaction/CombatInterface.h>

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute(); // 指定要捕获的属性种类为 Intelligence
	IntelligenceDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; // 指定从目标（承受效果的一方）身上取值
	IntelligenceDef.bSnapshot = false; // 不使用快照，每次计算都取当前最新的实时数值

	RelevantAttributesToCapture.Add(IntelligenceDef);// 把 VigorDef 加入本次计算所需捕获的属性列表
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); // 获取发起者（Source）身上聚合的 GameplayTag 容器
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags(); // 获取承受者（Target）身上聚合的 GameplayTag 容器

	FAggregatorEvaluateParameters EvaluationParameters; // 创建一个"计算求值参数"结构体，用于传给捕获取值函数
	EvaluationParameters.SourceTags = SourceTags; // 把 Source 的标签塞进求值参数，供条件判断/加成计算使用
	EvaluationParameters.TargetTags = TargetTags; // 把 Target 的标签塞进求值参数，供条件判断/加成计算使用

	float Intelligence = 0.f; // 声明一个浮点变量，用来接收捕获到的 Vigor 数值，先初始化为 0
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluationParameters, Intelligence); // 根据 VigorDef 的定义，从 Spec 中实际取出 Vigor 的当前数值，写入 Vigor 变量
	Intelligence = FMath::Max<float>(Intelligence, 0.f);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject()); // 从效果上下文中取出来源对象，并尝试转换为 ICombatInterface 接口指针
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel(); // 通过接口调用，获取该对象（发起者）当前的等级

	return 20 + 2 * Intelligence + 5 * PlayerLevel;
}
