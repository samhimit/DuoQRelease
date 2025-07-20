#include "Characters/Player/PlayerCrowdAgentInterface.h"

// Sets default values for this component's properties
UPlayerCrowdAgentInterface::UPlayerCrowdAgentInterface()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPlayerCrowdAgentInterface::BeginPlay()
{
	Super::BeginPlay();

	UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
	if (CrowdManager)
	{
		ICrowdAgentInterface* IAgent = Cast<ICrowdAgentInterface>(this);
		CrowdManager->RegisterAgent(IAgent);
	}
}

// Called every frame
void UPlayerCrowdAgentInterface::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerCrowdAgentInterface::InitializeComponent()
{
	Super::InitializeComponent();
}

FVector UPlayerCrowdAgentInterface::GetCrowdAgentLocation() const
{
	return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
}
FVector UPlayerCrowdAgentInterface::GetCrowdAgentVelocity() const
{
	return GetOwner() ? GetOwner()->GetVelocity() : FVector::ZeroVector;
}
void UPlayerCrowdAgentInterface::GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const
{
	if (GetOwner())
	{
		GetOwner()->GetSimpleCollisionCylinder(CylinderRadius, CylinderHalfHeight);
	}
}
float UPlayerCrowdAgentInterface::GetCrowdAgentMaxSpeed() const
{
	return 300;
}
