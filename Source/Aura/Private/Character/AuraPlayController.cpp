// Mr.Wang


#include "Character/AuraPlayController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "Components/DecalComponent.h"
#include "UI/Widget/DamageTextComponent.h"



AAuraPlayController::AAuraPlayController()
{
	bReplicates = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CursorTrace();
	if (BAutoRunning)
	{
		AutoRun();
	}
	UpdateMagicCircleLocation();
}

void AAuraPlayController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial!=nullptr)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0,DecalMaterial);
		}
	}
}

void AAuraPlayController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayController::ShowDamageNumber_Implementation(float DamageNumber,ACharacter* TargetCharacter,bool IsBlockHit,bool IsCriticalHit)
{
	if (IsValid(TargetCharacter)&&DamageTextComponentClass&&IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter,DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageNumber,IsBlockHit,IsCriticalHit);
	}
}

void AAuraPlayController::AutoRun()
{
	if (APawn* ControllerPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControllerPawn->GetActorLocation(),ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline,ESplineCoordinateSpace::World);
		ControllerPawn->AddMovementInput(Direction);
		
		const float DistanceToDestination = (LocationOnSpline-CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			BAutoRunning = false;
		}
	}
}

void AAuraPlayController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}


void AAuraPlayController::CursorTrace()
{
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->UnHighlightActor();
		LastActor = nullptr;
		ThisActor = nullptr;
		
		return;
	}
	
	
	ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel,false,CursorHit);
	if (!CursorHit.bBlockingHit) return;//检查是否碰撞被阻塞，也就是是否命中
	
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	/*
	 * 光标进行线性追踪的几种情况
	 * a.两个Actor都是null的
	 *	--什么都不需要做
	 *b.LastActor is null&&ThisActor is valid
	 *	--第一次追踪到Actor，将他进行高亮
	 *c.LastActor is Valid&&thisActor is null
	 *  --取消Actor的高亮
	 *d.both actor is valid but ThisActor != LastActor
	 *	--取消LastActor的高亮，使其ThisActor高亮
	 *e.both actor is valid,and the same;
	 *  --第一次已经高亮了，所以不需要再进行高亮了
	 */
	if (LastActor!=ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAuraPlayController::AbilityInputPressed(FGameplayTag InputTag)
{
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		BAutoRunning = false;
	}
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
	
}

void AAuraPlayController::AbilityInputReleased(FGameplayTag InputTag)
{
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	if (!bTargeting&&!IsShiftKeyDown)
	{
		APawn* ControllerPawn = GetPawn();
		if (ControllerPawn && FollowTime <= ShortPressThreshold)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this,ControllerPawn->GetActorLocation(),CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc,ESplineCoordinateSpace::World);
				}
				if (!NavPath->PathPoints.IsEmpty())
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num()-1];
				BAutoRunning = true;
			}
			if (GetASC()&&!GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ClickNiagaraSystem,CachedDestination);
			}
			
		}
		FollowTime = 0;
		bTargeting = false;
	}
	
	
}

void AAuraPlayController::AbilityInputHeld(FGameplayTag InputTag)
{
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (bTargeting||IsShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds(); 
		
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}
		if (APawn* ControllerPawn = GetPawn())
		{
			const FVector WorlderDirection = (CachedDestination-ControllerPawn->GetActorLocation()).GetSafeNormal();
			ControllerPawn->AddMovementInput(WorlderDirection);
		}
	}
	
}

UAuraAbilitySystemComponent* AAuraPlayController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
		
	}
	return AuraAbilitySystemComponent;
}


void AAuraPlayController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem )
		Subsystem->AddMappingContext(AuraContext,0);
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData); 
}

void AAuraPlayController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	
	AuraInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayController::Move);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Started,this,&AAuraPlayController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction,ETriggerEvent::Completed,this,&AAuraPlayController::ShiftReleased);
	AuraInputComponent->BindAbilityInput(InputConfig,this,&ThisClass::AbilityInputPressed,&ThisClass::AbilityInputReleased,&ThisClass::AbilityInputHeld);
	
}

void AAuraPlayController::Move(const FInputActionValue& InputActionValue)
{
	if (GetASC()&&GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	
	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardVector,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightVector,InputAxisVector.X);
	}
}


