


#include "Character/AuraCharacter.h"
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

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController); 

	InitAbilityActorInfo();//
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

	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);//Ë­ÊÇ Owner£¬Ë­ÊÇ Avatar
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}
