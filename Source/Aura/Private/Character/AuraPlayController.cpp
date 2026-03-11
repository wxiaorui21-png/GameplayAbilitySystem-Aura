// Mr.Wang


#include "Character/AuraPlayController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayController::AAuraPlayController()
{
	bReplicates = true;
}

void AAuraPlayController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CursorTrace();
}

void AAuraPlayController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
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
	if (LastActor == nullptr)
	{
		if (ThisActor == nullptr){
		}
		if (ThisActor != nullptr)
			ThisActor->HighlightActor();
	}else
	{
		if (ThisActor == nullptr)
			LastActor->UnHighlightActor();
		if (ThisActor != nullptr&&ThisActor != LastActor)
		{
			LastActor->UnHighlightActor();
			ThisActor->HighlightActor();
		}
		if (ThisActor != nullptr&&ThisActor == LastActor){}
	}
}

void AAuraPlayController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
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
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayController::Move);
}

void AAuraPlayController::Move(const FInputActionValue& InputActionValue)
{
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


