// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StarlightSeeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUOQ_API UStarlightSeeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStarlightSeeComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldAddToVisibleList = false;

	/** this is reserved for things starlight can't see but you still want their tags to be sent*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldAddToTagList = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Tags;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
