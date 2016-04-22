// Copyright 2016, Sam Jeeves. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Misc/DateTime.h"
#include "GoogleMapWidget.generated.h"


USTRUCT(BlueprintType)
struct FLocationStruct
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	float Latitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	float Longitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	FDateTime Time;

	FLocationStruct()
		: Latitude(0),
		Longitude(0),
		Time(0)
	{}

	FLocationStruct(float lat, float lng)
		: Latitude(lat),
		Longitude(lng),
		Time(FDateTime::Now())
	{}
};


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
