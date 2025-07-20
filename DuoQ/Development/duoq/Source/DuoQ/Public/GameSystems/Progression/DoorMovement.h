// Copyright 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorMovement.generated.h"

UENUM()
enum EDoorState { Closed, Opened, Opening, Closing };

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoorOpenedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDoorClosedSignature, float, SpeedOfClose);


//TODO: CLAMP AND ADD CAMERA SHAKE
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUOQ_API UDoorMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDoorMovement();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DoorOpenSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DoorGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DoorStoppingAcceleration;

	UPROPERTY(BlueprintAssignable)
	FDoorOpenedSignature OnDoorOpened;

	UPROPERTY(BlueprintAssignable)
	FDoorClosedSignature OnDoorClosed;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//returns whether door successfully reached position
	bool MoveDoorToPosition(FVector Target, float MaxDistanceDelta);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetupDoorMovement(USceneComponent* InDoor, FVector InOpenPosition, FVector InClosedPosition);
	
	UFUNCTION(BlueprintCallable)
	void Open();

	UFUNCTION(BlueprintCallable)
	void Close();

private:
	
	UPROPERTY()
	FVector DoorClosedPosition;

	UPROPERTY()
	FVector DoorOpenedPosition;

	UPROPERTY()
	USceneComponent* Door;

	EDoorState DoorState;

	UPROPERTY()
	float ZVelocity;

};
