#include "MyNameTagWidget.h"

void UMyNameTagWidget::SetLabel(const FString& InText)
{
	if (NameText)
		NameText->SetText(FText::FromString(InText));
}