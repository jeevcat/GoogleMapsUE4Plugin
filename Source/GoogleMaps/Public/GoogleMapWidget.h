// Copyright 2016, Sam Jeeves. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
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
	void UpdateSplit(float overDistance);

	bool initialised;
	
public:
	// Constructor declaration
	UGoogleMapWidget(const FObjectInitializer& ObjectInitializer);

	void LocationChanged(float lat, float lng);

	/** Has the Google API successfully established a GPS connection? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	bool GPSConnected;

	/** GPS data points */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	TArray<FLocationStruct> GPSPoints;

	/** Total distance in km */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
		float TotalDistance;

	/** Time taken to run last 1 km */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	FTimespan Split;

	/** Time when running began */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	FDateTime StartTime;



	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
		void ConnectToGoogleAPI();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
		void DisconnectFromGoogleAPI();

	/** New GPS location has been received */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Location Changed"))
		void OnLocationChanged(float Latitude, float Longitude);
	
	
};
