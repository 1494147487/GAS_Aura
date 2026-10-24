


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability Name - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</> \n<Default>Causes much more damage. </>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel)const
{
    float ManaCost = 0.f; // 最终要返回的法力消耗，默认 0
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect()) // 取技能配置的消耗 GE，没配则为空
    {
        for (FGameplayModifierInfo Mod : CostEffect->Modifiers) // 遍历消耗 GE 里的所有属性修改器
        {
            if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute()) // 只关心修改 Mana 的那一条
            {
                //如果这条修改器的数值是静态的（也就是直接填的 FScalableFloat，可以挂曲线表），就按传入的等级 InLevel 算出结果
                Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);//
                break;
            }
        }
    }

    return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel)const
{
    float Cooldown = 0.f;
    if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    return Cooldown;

}
