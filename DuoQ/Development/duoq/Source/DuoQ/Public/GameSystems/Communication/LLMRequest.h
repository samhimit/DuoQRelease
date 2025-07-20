// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LLMRequest.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLLMOnRequestComplete,class UVaRestJsonObject*, Request);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLLMOnRequestFail,class UVaRestRequestJSON*, Request);

/**
 * 
 */
UCLASS()
class DUOQ_API ULLMRequest : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FLLMOnRequestComplete OnRequestComplete;
	
	UPROPERTY(BlueprintAssignable)
	FLLMOnRequestFail OnRequestFail;

	UFUNCTION()
	virtual void SendRequest(FString ConversationHistory, FString Prompt, class UCommunicationSubsystem* CommunicationSubsystem);

	static ULLMRequest* CreateLLMRequest();
protected:
	UPROPERTY()
	TObjectPtr<UVaRestRequestJSON> VaRestRequest;
	
	UFUNCTION()
	virtual void CompleteRequest(class UVaRestRequestJSON* Request);

	UFUNCTION()
	virtual void FailRequest(class UVaRestRequestJSON* Request);
};
