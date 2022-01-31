// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SplineMeshComponent.h>
#include <Engine/DataTable.h>
#include <Components/TimelineComponent.h>

#include "GrindRailSpline.generated.h"

UENUM(BlueprintType)
enum class ESplineMeshType : uint8 
{
	DEFAULT UMETA(DisplayName="Default Mesh"),
	START UMETA(DisplayName="Start Mesh"),
	END UMETA(DisplayName="End Mesh")
};

USTRUCT(BlueprintType)

struct FSplineMeshDetails : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UMaterialInstance* DefaultMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UMaterialInstance* AlternateMaterial;

	FSplineMeshDetails() : ForwardAxis(ESplineMeshAxis::Type::X) {}
};

UCLASS()
class MORONAVIRUS_API AGrindRailSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrindRailSpline();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ProcessMovementTimeline(float Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	class USplineComponent* SplineComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spline")
	TMap<ESplineMeshType, FSplineMeshDetails> SplineMeshMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spline")
	UCurveFloat* MovementCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spline")
	float EjectionImpulseStrength = 150000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Spline")
	class UBoxComponent* StartTriggerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Spline")
	class UBoxComponent* EndTriggerComponent;

protected:

	FTimeline MovementTimeline;

	class AMoronavirusCharacter* Character;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> CartClass;

	AActor* Cart;

protected:

	UFUNCTION()
	void OnEndReached(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnStartReached(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndMovementTimeline();
};
