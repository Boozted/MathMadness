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

    struct FTargetData
    {
        ETargetOperator Op;
        int32 Value;
    };

    // build operator list based on settings
    TArray<ETargetOperator> Operators;
    Operators.Add(ETargetOperator::Add);
    if (bAllowSubtract) Operators.Add(ETargetOperator::Subtract);
    if (bAllowMultiply) Operators.Add(ETargetOperator::Multiply);
    if (bAllowDivide)   Operators.Add(ETargetOperator::Divide);

    TArray<FTargetData> TargetData;
    float RunningScore = 0.f;

    // always start with Add so there is a base score
    int32 FirstValue = FMath::RandRange(AddValueMin, AddValueMax);
    TargetData.Add({ ETargetOperator::Add, FirstValue });
    RunningScore += FirstValue;

    int32 Attempts = 0;
    while (FMath::Abs(RunningScore) < MinimumGoalScore && TargetData.Num() < MaxTargets && Attempts < 200)
    {
        Attempts++;

        ETargetOperator Op = Operators[FMath::RandRange(0, Operators.Num() - 1)];
        int32 Value = 0;

        if (Op == ETargetOperator::Divide)
        {
            TArray<int32> SafeDivisors = { 2, 3, 4, 5 };
            Value = SafeDivisors[FMath::RandRange(0, SafeDivisors.Num() - 1)];
        }
        else if (Op == ETargetOperator::Multiply)
        {
            Value = FMath::RandRange(MultiplyValueMin, MultiplyValueMax);
        }
        else if (Op == ETargetOperator::Subtract)
        {
            Value = FMath::RandRange(SubtractValueMin, SubtractValueMax);
        }
        else
        {
            Value = FMath::RandRange(AddValueMin, AddValueMax);
        }

        // simulate what this would do to the score
        float TestScore = RunningScore;
        switch (Op)
        {
            case ETargetOperator::Add:      TestScore += Value; break;
            case ETargetOperator::Subtract: TestScore -= Value; break;
            case ETargetOperator::Multiply: TestScore *= Value; break;
            case ETargetOperator::Divide:
                if (Value != 0) TestScore /= Value;
                break;
        }

        // only add if score stays positive
        if (TestScore > 0)
        {
            TargetData.Add({ Op, Value });
            RunningScore = TestScore;
        }
    }

    // pad with Add targets if still under minimum
    while (FMath::Abs(RunningScore) < MinimumGoalScore && TargetData.Num() < MaxTargets)
    {
        int32 Value = FMath::RandRange(AddValueMin, AddValueMax);
        TargetData.Add({ ETargetOperator::Add, Value });
        RunningScore += Value;
    }

    int32 GoalScore = MinimumGoalScore;

    AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->SetGoalScore(GoalScore);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
            FString::Printf(TEXT("Goal Score: %d | Max Possible: %d | Targets: %d"),
                GoalScore, FMath::RoundToInt(RunningScore), TargetData.Num()));
    }

    for (const FTargetData& Data : TargetData)
    {
        float RandomX = FMath::FRandRange(-SpawnAreaHalfSizeX, SpawnAreaHalfSizeX);
        float RandomY = FMath::FRandRange(-SpawnAreaHalfSizeY, SpawnAreaHalfSizeY);
        float RandomZ = SpawnHeightOffset + FMath::FRandRange(0.f, SpawnHeightVariance);

        FVector SpawnLocation = GetActorLocation() + FVector(RandomX, RandomY, RandomZ);
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