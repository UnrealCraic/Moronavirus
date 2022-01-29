// Fill out your copyright notice in the Description page of Project Settings.


#include "GrindRailSpline.h"

#include <Components/SplineComponent.h>

// Sets default values
AGrindRailSpline::AGrindRailSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	if (SplineComponent)
	{
		SetRootComponent(SplineComponent);
	}
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
		else if (EndMeshDetails && EndMeshDetails->Mesh && SplineCount == SplineComponent->GetNumberOfSplinePoints() - 1)
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
	
}

// Called every frame
void AGrindRailSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

