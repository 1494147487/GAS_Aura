

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,//携带了这次Execution需要用到的所有上下文信息
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;//在函数体内计算完之后,把结果通过这个参数"吐出去"
};
