// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CalloutHeartArrowWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class DUOQ_API UCalloutHeartArrowWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> HeartImage;
};
