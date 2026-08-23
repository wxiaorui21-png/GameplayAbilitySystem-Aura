// Mr.Wang


#include "Character/AuraCharacter.h"
#include "NiagaraComponent.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "Character/AuraPlayController.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemGlobals.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "Commandlets/WorldPartitionCommandletHelpers.h"
#include "Debuff/DebuffNiagaraCompoent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;//角色方向随移动方向转动
	GetCharacterMovement()->RotationRate = FRotator(0.f,400.f,0.f);
	GetCharacterMovement()->bConstrainToPlane = true;//将角色移动控制在一个平面上，对于2.5d俯视
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//将角色在开始时控制在一个平面上
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CharacterClass = ECharacterClass::Elementalist;
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(RootComponent);
	LevelUpNiagaraComponent->bAutoActivate = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Init Ability actor info for Server
	InitAbilityActorInfo();
	
	LoadProgress();
	
	if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameModeBase->LoadWorldState(GetWorld());
	}
}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameModeBase)
	{
		ULoadScreenSaveGame* SaveObject = AuraGameModeBase->RetrieveInGameData();
		if (SaveObject == nullptr) return;
		
		if (SaveObject->bFirstLoad)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}else
		{
			if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				ASC->AddCharacterAbilitiesFromSaveData(SaveObject);
			}
			
			
			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetLevel(SaveObject->PlayerLevel);
				AuraPlayerState->SetXP(SaveObject->XP);
				AuraPlayerState->SetAttributePoints(SaveObject->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveObject->SpellPoints);
			}
			
			UAuraAbilitySystemLibrary::InitializeDefaultAttributeFromSave(this,AbilitySystemComponent,SaveObject);
		}
	}
}


void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init Ability actor info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTagContainer NewContainer;
		NewContainer.AddTag(GameplayTags.Player_Block_CursorTrace);
		NewContainer.AddTag(GameplayTags.Player_Block_InputHeld);
		NewContainer.AddTag(GameplayTags.Player_Block_InputPressed);
		NewContainer.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			ASC->AddLooseGameplayTags(NewContainer);
			StunNiagaraCompoent->Activate();
		}
		else
		{
			ASC->RemoveLooseGameplayTags(NewContainer);
			StunNiagaraCompoent->Deactivate();	
		}
	}
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXp(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}	
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

int32 AAuraCharacter::FindLevelByXP_Implementation(int32 InXP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInformation->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInformation->LevelUpInfo[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInformation->LevelUpInfo[Level].SpellPointReward;
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC -> UpdateAbilityStates(AuraPlayerState->GetPlayerLevel());
	}
	
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayController* AuraPlayerController = Cast<AAuraPlayController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayController* AuraPlayerController = Cast<AAuraPlayController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SavePlayerInGameData_Implementation(const FName& PlayerStartTag)
{
	AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AuraGameModeBase)
	{
		ULoadScreenSaveGame* SaveObject = AuraGameModeBase->RetrieveInGameData();
		if (SaveObject == nullptr) return;
		//保存所需要的数据
		//两种：角色数据  世界状态（例如哪些检查点已经解锁）
		
		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveObject->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveObject->XP = AuraPlayerState->GetXP();
			SaveObject->SpellPoints=AuraPlayerState->GetSpellPoints();
			SaveObject->AttributePoints=AuraPlayerState->GetAttributePoints();
		}
		SaveObject->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		SaveObject->bFirstLoad = false;
		SaveObject->PlayerStart = PlayerStartTag;
		
		//存储能力
		if (!HasAuthority()) return;
		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
		FForEachAbility SaveAbilityDelegate;
		SaveObject->SavedAbilities.Empty();//先置为空再次循环加入能力
		SaveAbilityDelegate.BindLambda([this,ASC,SaveObject](const FGameplayAbilitySpec& Spec)
		{
			FSavedAbility SavedAbility;
			const FGameplayTag AbilityTag = ASC->GetAbilityTagFromSpec(Spec);
			const FGameplayTag AbilityStatus = ASC->GetStatusTagFromSpec(Spec);
			const FGameplayTag AbilitySlot = ASC->GetSlotTagFromTag(AbilityTag);
			FAuraAbilityInfo Info = UAuraAbilitySystemLibrary::GetAbilityInfo(this)->FindAbilityInfoByTag(AbilityTag);
			
			SavedAbility.AbilityLevel = Spec.Level;
			SavedAbility.AbilitySlot = AbilitySlot;
			SavedAbility.AbilityStatus = AbilityStatus;
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityType = Info.AbilityType;
			
			SaveObject->SavedAbilities.AddUnique(SavedAbility);//Unique防止加入重复的，需要重载FSavedAbility的==
		});
		ASC->ForEachAbility(SaveAbilityDelegate);
		
		//存储到磁盘中
		AuraGameModeBase->SaveInGameData(SaveObject);
	}
}


void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState,this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilitySystemInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun,EGameplayTagEventType::NewOrRemoved).AddUObject(this,&AAuraCharacter::StunnedTagChanged);
	
	DebuffGetASCDelegate.Broadcast(AbilitySystemComponent);
	if (AAuraPlayController* AuraPlayerController = Cast<AAuraPlayController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}//不使用check的目的是不希望程序崩溃
	
}


