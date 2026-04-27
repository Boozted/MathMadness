#include "AShootingTarget.h"
#include "Components/TextRenderComponent.h"
#include "ShooterGameMode.h"
#include "Engine/Engine.h"

AShootingTarget::AShootingTarget()
{
    PrimaryActorTick.bCanEverTick = true;

    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
    RootComponent = TargetMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
        TEXT("/Game/Fab/Wooden_target_01v1/wooden_target1v1/StaticMeshes/wooden_target1v1.wooden_target1v1"));
    if (MeshAsset.Succeeded())
    {
        TargetMesh->SetStaticMesh(MeshAsset.Object);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AShootingTarget: Could not find target mesh"));
    }

    TargetMesh->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

    LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
    LabelText->SetupAttachment(RootComponent);
}

void AShootingTarget::BeginPlay()
{
    Super::BeginPlay();

    TargetMesh->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

    if (!LabelText) return;

    LabelText->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    LabelText->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    LabelText->SetHorizontalAlignment(EHTA_Center);
    LabelText->SetVerticalAlignment(EVRTA_TextCenter);
    LabelText->SetWorldSize(15.f);

    TArray<FColor> VibrantColors = {
        FColor::Red,
        FColor::Green,
        FColor::Blue,
        FColor::Yellow,
        FColor::Cyan,
        FColor::Magenta,
        FColor(255, 128, 0),
        FColor(128, 0, 255),
        FColor(0, 255, 128),
        FColor(255, 0, 128)
    };

    int32 ColorIndex = FMath::RandRange(0, VibrantColors.Num() - 1);
    LabelText->SetTextRenderColor(VibrantColors[ColorIndex]);
}

void AShootingTarget::SetOperatorAndValue(ETargetOperator InOperator, int32 InValue)
{
    Operator = InOperator;
    TargetValue = InValue;

    FString OpSymbol;
    switch (Operator)
    {
        case ETargetOperator::Add:      OpSymbol = TEXT("+"); break;
        case ETargetOperator::Subtract: OpSymbol = TEXT("-"); break;
        case ETargetOperator::Multiply: OpSymbol = TEXT("x"); break;
        case ETargetOperator::Divide:   OpSymbol = TEXT("/"); break;
    }

    SetLabelText(FString::Printf(TEXT("%s%d"), *OpSymbol, TargetValue));
}

void AShootingTarget::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AShootingTarget::SetLabelText(const FString& NewText)
{
    if (LabelText)
    {
        LabelText->SetText(FText::FromString(NewText));
    }
}

void AShootingTarget::OnHit()
{
    AShooterGameMode* GM = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->ApplyTargetOperator(Operator, TargetValue);

        FString Message = FString::Printf(TEXT("Target hit! Score: %d / Goal: %d"),
            GM->GetScore(), GM->GetGoalScore());
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Message);
    }

    Destroy();
}