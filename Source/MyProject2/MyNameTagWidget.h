#pragma once
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MyNameTagWidget.generated.h"

UCLASS()
class MYPROJECT2_API UMyNameTagWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	void SetLabel(const FString& InText);
};