

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UAuraAssetManager& Get();

protected:

	//// 重写引擎的初始资产加载入口。
	// 该函数在引擎启动流程中被调用的时机极早，早于游戏世界(World)创建、
	// 早于第一个 Actor 生成，因此适合在这里做"必须最先完成"的全局初始化工作。
	virtual void StartInitialLoading()override;
};
