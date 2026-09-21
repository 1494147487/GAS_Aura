


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()//
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	UAbilitySystemGlobals::Get().InitGlobalData();//:获取 GAS 系统里那个全局唯一的"配置/管理"对象,让它去加载一些整个游戏都要用到的全局数据表
}
