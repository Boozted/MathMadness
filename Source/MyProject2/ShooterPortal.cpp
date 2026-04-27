#include "ShooterPortal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterGameMode.h"
#include "ShooterCharacter.h"
#include "Engine/Engine.h"

AShooterPortal::AShooterPortal()
{
    PrimaryActorTick.bCanEverTick = false;

    PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
    RootComponent = PortalMesh;
    PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetBoxExtent(FVector(50.f, 50.f, 100.f));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AShooterPortal::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AShooterPortal::OnOverlapBegin);
}

void AShooterPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // only trigger for the player
    if (!Cast<AShooterCharacter>(OtherActor)) return;

    AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    if (GM->GetScore() >= GM->GetGoalScore())
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
            FString::Printf(TEXT("Score %d reached goal %d! Entering portal..."),
                GM->GetScore(), GM->GetGoalScore()));

        UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
            FString::Printf(TEXT("Need %d more points to enter! (%d / %d)"),
                GM->GetGoalScore() - GM->GetScore(),
                GM->GetScore(),
                GM->GetGoalScore()));
    }
}