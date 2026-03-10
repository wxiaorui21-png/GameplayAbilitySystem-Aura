// Mr.Wang


#include "Character/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;//角色方向随移动方向转动
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);
	GetCharacterMovement()->bConstrainToPlane = true;//将角色移动控制在一个平面上，对于2.5d俯视
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//将角色在开始时控制在一个平面上
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}
