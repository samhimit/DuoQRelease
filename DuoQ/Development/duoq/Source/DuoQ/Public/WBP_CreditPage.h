// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_CreditPage.generated.h"

class UCreditPairWidget;
struct FCreditPage;
class UCreditsWidget;
/**
 * 
 */
UCLASS()
class DUOQ_API UWBP_CreditPage : public UUserWidget
{
	GENERATED_BODY()

	void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void FillCredits(FCreditPage CreditPage);
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TArray<TObjectPtr<UCreditPairWidget>> CreditPairs;
};
