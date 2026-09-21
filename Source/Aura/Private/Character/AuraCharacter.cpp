


#include "Character/AuraCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	AutoPossessAI = EAutoPossessAI::Disabled;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetCharacterMovement()->bConstrainToPlane = true;

	GetCharacterMovement()->bSnapToPlaneAtStart = true;


	bUseControllerRotationPitch = false;

	bUseControllerRotationRoll = false;
	
	bUseControllerRotationYaw = false;

}

int32 AAuraCharacter::GetPlayerLevel()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); 

	InitAbilityActorInfo();//
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState(); 
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()//
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!AuraPlayerState)
	{
		return;
	}

	UAbilitySystemComponent* ASC = AuraPlayerState->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	ASC->InitAbilityActorInfo(AuraPlayerState, this);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
	{
		AuraASC->AbilityActorInfoSet();// ASC 就绪后绑定 GE 应用回调，用来广播资源标签
	}

	AbilitySystemComponent = ASC;
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	InitializeDefaultAttributes();//
}
