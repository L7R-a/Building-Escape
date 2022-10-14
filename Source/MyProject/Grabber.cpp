// Diego La Rosa 2022
#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandle();
	SetupInputComponent();
}
void UGrabber::SetupInputComponent()
	{
		InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
		InputRelease = GetOwner()->FindComponentByClass<UInputComponent>();
		if(InputComponent)
		{
			InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		}
		if(InputRelease)
		{
			InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
		}

	}
void UGrabber::FindPhysicsHandle()
	{
		PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

		if(PhysicsHandle == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("The %s is missing a PhysicsHandle component!"), *GetOwner()->GetName());
		}
	}
void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if(ActorHit)
	{
		if(!PhysicsHandle)
		{
			return;
		}
		PhysicsHandle->GrabComponentAtLocation( ComponentToGrab,NAME_None, GetLineTraceEnd() );
	}
}
void UGrabber::Release()

{
	PhysicsHandle->ReleaseComponent();
}
FVector UGrabber:: GetLineTraceEnd() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	return LineTraceEnd;
}
FVector UGrabber::GetPlayerViewPointLocation() const
{

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);
	return PlayerViewPointLocation;
}
// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PhysicsHandle)
	{
		return;
	}
	if(PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}

FHitResult UGrabber:: GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType( 
		OUT Hit,
		GetPlayerViewPointLocation(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);
	return Hit;
}
