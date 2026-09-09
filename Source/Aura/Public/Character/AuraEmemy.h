

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
	virtual void HighlightActor()override;
	virtual void UnHighlightActor()override;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo()override;
};
