

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AuraGameplayTags.h"
#include <AbilitySystem/Data/AbilityInfo.h>
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */

struct FSelectedAbility // 记录当前选中的技能：是哪个技能、处于什么状态
{
	FGameplayTag Ability = FGameplayTag(); // 选中技能的标签
	FGameplayTag Status = FGameplayTag(); // 选中技能当前的状态标签（Locked/Eligible/Unlocked/Equipped）
};

 // 选中技能球时通知UI的动态多播委托：告诉UI"加点按钮"和"装备按钮"各自是否可用
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellGlobeSelectedSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled, FString, DescriptionString, FString, NextLevelDescriptionString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature,const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityType);

UCLASS(BlueprintType, Blueprintable)//声明一个可以在蓝图中使用的类，允许蓝图继承和实例化它
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	

public:
	virtual void BroadcastInitialValues()override;
	virtual void BindCallbacksToDependencies()override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature SpellPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitingForEquipDelegate;

	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void GlobleDeselect();

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);
private:
	// 根据技能状态和剩余技能点，计算"加点按钮"和"装备按钮"是否应该启用（结果通过引用参数传出）
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpellPointsButton, bool& bShouldEnableEquipButton);


	FSelectedAbility SelectedAbility = { FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked }; // 初始状态：没有选中任何技能，状态为锁定

	int32 CurrentSpellPoints = 0;

	bool bWaitingForEquipSelection = false;

	FGameplayTag SelectedSlot;//当前选中的装备栏槽位的 Tag

	

};
