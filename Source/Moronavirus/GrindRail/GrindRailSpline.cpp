// Fill out your copyright notice in the Description page of Project Settings.


#include "GrindRailSpline.h"

#include <Components/SplineComponent.h>
#include "../MoronavirusCharacter.h"
#include <Components/BoxComponent.h>

// Sets default values
AGrindRailSpline::AGrindRailSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	if (SplineComponent)
	{
		SplineComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SplineComponent->SetGenerateOverlapEvents(true);
		SetRootComponent(SplineComponent);
	}

	StartTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("StartTriggerComponent"));
	StartTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StartTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrindRailSpline::OnStartReached);

	EndTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("EndTriggerComponent"));
	EndTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EndTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrindRailSpline::OnEndReached);
}

void AGrindRailSpline::OnConstruction(const FTransform& Transform)
{
	if (SplineMeshMap.Num() == 0)
	{
		return;
	}

	FSplineMeshDetails* DefaultMeshDetails = nullptr;

	if (SplineMeshMap.Contains(ESplineMeshType::DEFAULT))
	{
		DefaultMeshDetails = SplineMeshMap.Find(ESplineMeshType::DEFAULT);
	}
	else {
		return;
	}

	FSplineMeshDetails* StartMeshDetails = nullptr;

	if (SplineMeshMap.Contains(ESplineMeshType::START))
	{
		StartMeshDetails = SplineMeshMap.Find(ESplineMeshType::START);
	}

	FSplineMeshDetails* EndMeshDetails = nullptr;

	if (SplineMeshMap.Contains(ESplineMeshType::END))
	{
		EndMeshDetails = SplineMeshMap.Find(ESplineMeshType::END);
	}

	for (int SplineCount = 0; SplineCount < SplineComponent->GetNumberOfSplinePoints() - 1; SplineCount++)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		if (StartMeshDetails && StartMeshDetails->Mesh && SplineCount == 0)
		{
			SplineMeshComponent->SetStaticMesh(StartMeshDetails->Mesh);
			SplineMeshComponent->SetForwardAxis(StartMeshDetails->ForwardAxis);
		}
		else if (EndMeshDetails && EndMeshDetails->Mesh && SplineCount == SplineComponent->GetNumberOfSplinePoints() - 2)
		{
			SplineMeshComponent->SetStaticMesh(EndMeshDetails->Mesh);
			SplineMeshComponent->SetForwardAxis(EndMeshDetails->ForwardAxis);
		}
		else if(DefaultMeshDetails->Mesh) 
		{
			SplineMeshComponent->SetStaticMesh(DefaultMeshDetails->Mesh);
			SplineMeshComponent->SetForwardAxis(DefaultMeshDetails->ForwardAxis);
			
			if (DefaultMeshDetails->AlternateMaterial && SplineCount > 0 && SplineCount % 2 == 1)
			{
				SplineMeshComponent->SetMaterial(0, DefaultMeshDetails->AlternateMaterial);
			}
			else if (DefaultMeshDetails->DefaultMaterial)
			{
				SplineMeshComponent->SetMaterial(0, DefaultMeshDetails->DefaultMaterial);
			}

		}
		
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
		const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);

		SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

		SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

// Called when the game starts or when spawned
void AGrindRailSpline::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Cart = GetWorld()->SpawnActor<AActor>(CartClass, Params);
}

void AGrindRailSpline::ProcessMovementTimeline(float Value)
{
	const float SplineLength = SplineComponent->GetSplineLength();

	FVector CurrentSplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);
	FRotator CurrentSplineRotator = SplineComponent->GetRotationAtDistanceAlongSpline(Value * SplineLength, ESplineCoordinateSpace::World);

	if (MovementTimeline.IsReversing())
	{
		CurrentSplineRotator.Yaw += 180;
	}
	CurrentSplineLocation.Z += Character->GetSimpleCollisionHalfHeight();
	Character->SetActorLocation(CurrentSplineLocation);
	Character->SetActorRotation(CurrentSplineRotator);
}

// Called every frame
void AGrindRailSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementTimeline.IsPlaying())
	{
		MovementTimeline.TickTimeline(DeltaTime);
	}
}

void AGrindRailSpline::OnEndReached(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (MovementTimeline.IsPlaying()) return;

	Character = Cast<AMoronavirusCharacter>(OtherActor);

	if (!Character) return;

	Character->SetIsGrinding(true);

	if (IsValid(Cart))
	{
		Character->AttachToActor(Cart, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//Character->AddActorLocalOffset(FVector(0, 0, 550));
	}

	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));

	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);

	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	MovementTimeline.ReverseFromEnd();
}

void AGrindRailSpline::OnStartReached(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (MovementTimeline.IsPlaying()) return;
	
	Character = Cast<AMoronavirusCharacter>(OtherActor);

	if (!Character) return;


	Character->SetIsGrinding(true);

	if (IsValid(Cart))
	{
		Character->AttachToActor(Cart, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//Character->AddActorLocalOffset(FVector(0, 0, 550));
	}

	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("ProcessMovementTimeline"));


	FOnTimelineEvent OnTimelineFinishedFunction;
	OnTimelineFinishedFunction.BindUFunction(this, TEXT("OnEndMovementTimeline"));

	MovementTimeline.AddInterpFloat(MovementCurve, ProgressFunction);
	MovementTimeline.SetTimelineFinishedFunc(OnTimelineFinishedFunction);

	MovementTimeline.SetTimelineLengthMode(TL_LastKeyFrame);
	MovementTimeline.PlayFromStart();

}

void AGrindRailSpline::OnEndMovementTimeline()
{
	Character->SetIsGrinding(false);
	Character->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	FRotator ResetRot = Character->GetActorRotation();

	ResetRot.Pitch = 0;
	Character->SetActorRotation(ResetRot);
}

