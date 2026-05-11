// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Shooter/ShooterGameMode.h"
#include "ShooterUI.h"
#include "AShootingTarget.h"
#include "ShooterPortal.h"
#include "ShooterGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

void AShooterGameMode::BeginPlay()
{
    Super::BeginPlay();

    ShooterUI = CreateWidget<UShooterUI>(UGameplayStatics::GetPlayerController(GetWorld(), 0), ShooterUIClass);
    ShooterUI->AddToViewport(0);

    if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
    {
        LevelNumber = GI->LevelNumber;
        UE_LOG(LogTemp, Warning, TEXT("Level %d | Multiplier: %.2f"), LevelNumber, GI->GoalScoreMultiplier);
        
        
    }
    
    
    TimeRemaining = StartingTime;

    
    float LevelPenalty = (LevelNumber - 1) * 5.f;
    TimeRemaining = FMath::Max(30.f, TimeRemaining - LevelPenalty);

    GetWorld()->GetTimerManager().SetTimer(CountdownTimer, this, &AShooterGameMode::TickCountdown, 1.f, true);
}

void AShooterGameMode::IncrementTeamScore(uint8 TeamByte)
{
    int32 Score = 0;
    if (int32* FoundScore = TeamScores.Find(TeamByte))
    {
        Score = *FoundScore;
    }

    ++Score;
    TeamScores.Add(TeamByte, Score);

    ShooterUI->BP_UpdateScore(TeamByte, Score);
}

void AShooterGameMode::AddScore(int32 Amount)
{
    CurrentScore += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Score: %d / Goal: %d"), GetScore(), GoalScore);
    CheckGoal();
}

void AShooterGameMode::ApplyTargetOperator(ETargetOperator Op, int32 Value)
{
    switch (Op)
    {
    case ETargetOperator::Add:
        CurrentScore += Value;
        break;
    case ETargetOperator::Subtract:
        CurrentScore -= Value;
        break;
    case ETargetOperator::Multiply:
        CurrentScore *= Value;
        break;
    case ETargetOperator::Divide:
        if (Value != 0) CurrentScore /= Value;
        break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Score: %d / Goal: %d"), GetScore(), GoalScore);
    CheckGoal();
}

void AShooterGameMode::SetGoalScore(int32 InGoal)
{
    GoalScore = InGoal;

    if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
    {
        LevelNumber = GI->LevelNumber;

        
        GoalScore = FMath::RoundToInt(GoalScore * GI->GoalScoreMultiplier);

        
        if (GI->LastGoalScore > 0)
        {
            int32 MinGoal = FMath::RoundToInt(GI->LastGoalScore * 1.05f);
            GoalScore = FMath::Max(GoalScore, MinGoal);
        }

        
        GI->LastGoalScore = GoalScore;
    }

    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
        FString::Printf(TEXT("Level %d | Goal Score: %d"), LevelNumber, GoalScore));
}

void AShooterGameMode::ApplyGoalMultiplier()
{
    if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
    {
        int32 OldGoal = GoalScore;
        GoalScore = FMath::RoundToInt(GoalScore * GI->GoalScoreMultiplier);
        UE_LOG(LogTemp, Warning, TEXT("Goal before multiplier: %d | After: %d | Multiplier: %.2f"), 
            OldGoal, GoalScore, GI->GoalScoreMultiplier);
    }
}

void AShooterGameMode::CheckGoal()
{
    if (bGoalReached) return;

    UE_LOG(LogTemp, Warning, TEXT("CheckGoal: Score=%d GoalScore=%d bGoalReached=%d"), 
        GetScore(), GoalScore, bGoalReached);

    if (GetScore() >= GoalScore)
    {
        bGoalReached = true;

        LogScoreToFile();

        if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
        {
            GI->AdvanceLevel();
        }

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
            FString::Printf(TEXT("Goal reached! Head to the portal. Score: %d / %d"), GetScore(), GoalScore));

        TArray<AActor*> Portals;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterPortal::StaticClass(), Portals);

        for (AActor* Portal : Portals)
        {
            if (!IsValid(Portal)) continue;
            if (AShooterPortal* P = Cast<AShooterPortal>(Portal))
            {
                P->ShowPortal();
            }
        }
    }
}

void AShooterGameMode::LogScoreToFile()
{
    FString LogEntry = FString::Printf(TEXT("Level: %d | Goal: %d | Final Score: %d\n"),
        LevelNumber, GoalScore, GetScore());

    FString FilePath = FPaths::ProjectDir() + TEXT("ScoreLog.txt");

    
    uint32 WriteFlags = LevelNumber == 1 ? FILEWRITE_None : FILEWRITE_Append;

    FFileHelper::SaveStringToFile(
        LogEntry,
        *FilePath,
        FFileHelper::EEncodingOptions::AutoDetect,
        &IFileManager::Get(),
        WriteFlags
    );

    UE_LOG(LogTemp, Warning, TEXT("Score logged to: %s"), *FilePath);
}
void AShooterGameMode::TickCountdown()
{
    if (bGoalReached) return;

    TimeRemaining -= 1.f;

    GEngine->AddOnScreenDebugMessage(1, 1.1f, FColor::White,
        FString::Printf(TEXT("Time: %.0f seconds"), TimeRemaining));

    if (TimeRemaining <= 10.f)
    {
        GEngine->AddOnScreenDebugMessage(2, 1.1f, FColor::Red,
            TEXT("Hurry up!"));
    }

    if (TimeRemaining <= 0.f)
    {
        OnTimerExpired();
    }
}

void AShooterGameMode::OnTimerExpired()
{
    GetWorld()->GetTimerManager().ClearTimer(CountdownTimer);

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Time's up! Restarting..."));

    
    if (UShooterGameInstance* GI = Cast<UShooterGameInstance>(GetGameInstance()))
    {
        GI->LevelNumber = 1;
        GI->GoalScoreMultiplier = 1.0f;
    }

    UGameplayStatics::OpenLevel(GetWorld(), FName("Lvl_Shooter"));
}