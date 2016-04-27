// Copyright 2016, Sam Jeeves. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Misc/DateTime.h"
#include "GoogleMapWidget.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UGoogleMapWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	void CreateGoogleMap(FVector2D Position, FVector2D Size);
	void RemoveGoogleMap();

	bool initialised;
	
public:
	// Constructor declaration
	UGoogleMapWidget(const FObjectInitializer& ObjectInitializer);

	// Overrides
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
};
