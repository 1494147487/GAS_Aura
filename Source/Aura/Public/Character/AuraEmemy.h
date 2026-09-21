

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEmemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEmemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:

	AAuraEmemy();

	/*EnemyInterface*/
	virtual void HighlightActor()override;
	virtual void UnHighlightActor()override;
	/*EnemyInterface*/

	/*CombatInterface*/
	virtual int32 GetPlayerLevel()override;
	/*CombatInterface*/

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo()override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
};
