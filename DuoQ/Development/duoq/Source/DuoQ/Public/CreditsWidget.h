// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditsWidget.generated.h"

class UWBP_CreditPage;
/**
 * 
 */
UCLASS()
class DUOQ_API UCreditsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void StartCredits();

	UFUNCTION(BlueprintCallable)
	void ClearCredits();
	
	UFUNCTION(BlueprintCallable)
	UWBP_CreditPage* GetCreditPage(int i);

	UFUNCTION(BlueprintCallable)
	void AddCreditPage(UWBP_CreditPage* Page);
	
	TMap<int,TArray<TObjectPtr<UWBP_CreditPage>>> CreditPages;
};
