#include "ShootingTargetSpawner.h"
#include "AShootingTarget.h"
#include "ShooterGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

AShootingTargetSpawner::AShootingTargetSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugMesh"));
    RootComponent = DebugMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (MeshAsset.Succeeded())
    {
        DebugMesh->SetStaticMesh(MeshAsset.Object);
    }
    DebugMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void AShootingTargetSpawner::BeginPlay()
{
    Super::BeginPlay();
    SpawnTargets();
}

void AShootingTargetSpawner::SpawnTargets()
{
    if (!TargetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShootingTargetSpawner: TargetClass is not set"));
        return;
    }

    int32 NumTargets = FMath::RandRange(MinTargets, MaxTargets);

    TArray<ETargetOperator> Operators = {
        ETargetOperator::Add,
        ETargetOperator::Subtract,
        ETargetOperator::Multiply,
        ETargetOperator::Divide
    };

    struct FTargetData
    {
        ETargetOperator Op;
        int32 Value;
    };

    TArray<FTargetData> TargetData;

    TargetData.Add({ ETargetOperator::Add, FMath::RandRange(10, 50) });

    for (int32 i = 1; i < NumTargets; i++)
    {
        ETargetOperator Op = Operators[FMath::RandRange(0, Operators.Num() - 1)];
        int32 Value = 0;

        if (Op == ETargetOperator::Divide)
        {
            TArray<int32> SafeDivisors = { 2, 3, 4, 5 };
            Value = SafeDivisors[FMath::RandRange(0, SafeDivisors.Num() - 1)];
        }
        else if (Op == ETargetOperator::Multiply)
        {
            Value = FMath::RandRange(2, 5);
        }
        else
        {
            Value = FMath::RandRange(10, 50);
        }

        TargetData.Add({ Op, Value });
    }

    float RunningScore = 0.f;
    for (const FTargetData& Data : TargetData)
    {
        switch (Data.Op)
        {
            case ETargetOperator::Add:      RunningScore += Data.Value; break;
            case ETargetOperator::Subtract: RunningScore -= Data.Value; break;
            case ETargetOperator::Multiply: RunningScore *= Data.Value; break;
            case ETargetOperator::Divide:
                if (Data.Value != 0) RunningScore /= Data.Value;
                break;
        }
    }

    int32 GoalScore = FMath::Abs(FMath::RoundToInt(RunningScore * 0.7f));

    AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->SetGoalScore(GoalScore);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
            FString::Printf(TEXT("Goal Score: %d"), GoalScore));
    }

    for (const FTargetData& Data : TargetData)
    {
        float RandomX = FMath::FRandRange(-SpawnAreaHalfSize, SpawnAreaHalfSize);
        float RandomY = FMath::FRandRange(-SpawnAreaHalfSize, SpawnAreaHalfSize);

        FVector SpawnLocation = GetActorLocation() + FVector(RandomX, RandomY, 0.f);
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AShootingTarget* Target = GetWorld()->SpawnActor<AShootingTarget>(
            TargetClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (Target)
        {
            Target->SetOperatorAndValue(Data.Op, Data.Value);
        }
    }
}