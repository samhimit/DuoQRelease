// Fill out your copyright notice in the Description page of Project Settings.


#include "IntroSequence/MainMenuPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameSystems/Communication/CommunicationSubsystem.h"
#include "GameSystems/Dialogue/BackupInputComponent.h"
#include "InputActionValue.h"

// Sets default values
AMainMenuPawn::AMainMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	BackupInputComponent = CreateDefaultSubobject<UBackupInputComponent>(TEXT("BackupInputComponent"));
}

void AMainMenuPawn::UnlockMicInput()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called when the game starts or when spawned
void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMainMenuPawn::UnMute()
{
	if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		CommunicationSubsystem->ResumeAudioCapture();
	}
}

void AMainMenuPawn::Mute()
{
	if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		CommunicationSubsystem->PauseAudioCapture();
	}
}

void AMainMenuPawn::ToggleMute()
{
	if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
	{
		if(CommunicationSubsystem->IsRecording())
		{
			CommunicationSubsystem->PauseAudioCapture();
		}
		else
		{
			CommunicationSubsystem->ResumeAudioCapture();
		}
	}
}

void AMainMenuPawn::ControlOptions(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	BackupInputComponent->HandleBackupInput(LookAxisVector);
}

void AMainMenuPawn::BackupInputStart()
{
	BackupInputComponent->BackupInputStart();
}

void AMainMenuPawn::BackupInputRelease()
{
	BackupInputComponent->BackupInputRelease();
}

void AMainMenuPawn::SetupPushToTalkToggle(bool bToggle)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		bool bRemovedControl = false;
		for(auto& Binding : TalkBindings)
		{
			bRemovedControl = EnhancedInputComponent->RemoveBindingByHandle(Binding) || bRemovedControl;
		}
		
		if(bRemovedControl)
		{
			if (UCommunicationSubsystem* CommunicationSubsystem = GetGameInstance()->GetSubsystem<UCommunicationSubsystem>())
			{
				CommunicationSubsystem->PauseAudioCapture();
			}
		}
		
		if(bToggle)
		{
			TalkBindings.Emplace(EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Started, this, &AMainMenuPawn::ToggleMute).GetHandle());
		}
		else
		{
			TalkBindings.Emplace(EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Started, this, &AMainMenuPawn::UnMute).GetHandle());
			TalkBindings.Emplace(EnhancedInputComponent->BindAction(TalkAction, ETriggerEvent::Completed, this, &AMainMenuPawn::Mute).GetHandle());
		}
	}
}

// Called every frame
void AMainMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	SetupPushToTalkToggle(false);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(BackupInputAction, ETriggerEvent::Started,this,&AMainMenuPawn::BackupInputStart);
		EnhancedInputComponent->BindAction(BackupInputAction, ETriggerEvent::Completed,this,&AMainMenuPawn::BackupInputRelease);

		EnhancedInputComponent->BindAction(ControlBackupInputAction, ETriggerEvent::Triggered, this, &AMainMenuPawn::ControlOptions);
	}
}

