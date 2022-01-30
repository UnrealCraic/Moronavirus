// Fill out your copyright notice in the Description page of Project Settings.


#include "GrindRailObstacle.h"
#include "../MoronavirusCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Components/BoxComponent.h>
#include <Components/StaticMeshComponent.h>

// Sets default values
AGrindRailObstacle::AGrindRailObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(Mesh);

	TriggerMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerMeshComp"));
	TriggerMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerMesh->SetGenerateOverlapEvents(true);
	TriggerMesh->OnComponentBeginOverlap.AddDynamic(this, &AGrindRailObstacle::OnBeginOverlap);
}

void AGrindRailObstacle::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMoronavirusCharacter* Character = Cast<AMoronavirusCharacter>(OtherActor);
	if (Character)
	{
		UGameplayStatics::ApplyDamage(Character, Damage, nullptr, this,UDamageType::StaticClass());
		Mesh->SetVisibility(false, true);
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AGrindRailObstacle::Respawn, RespawnRate, false);
		if (DestroySound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DestroySound, GetActorLocation());
		}
	}
}

void AGrindRailObstacle::Respawn()
{
	Mesh->SetVisibility(true, true);
}

// Called when the game starts or when spawned
void AGrindRailObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrindRailObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


