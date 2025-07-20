// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "DuoQGameInstance.generated.h"

class UDuoQSaveSystem;
class UDialogueSubsystem;
/**
 * 
 */
UCLASS()
class DUOQ_API UDuoQGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	UDialogueSubsystem* GetDialogueSubsystem();

protected:
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	TArray<TSubclassOf<USubsystem>> BPSubsystems;
	
};
