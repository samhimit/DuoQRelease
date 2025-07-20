// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BotCounterWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUOQ_API UBotCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
};
