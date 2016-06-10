// Copyright 2016, Sam Jeeves. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Misc/DateTime.h"
#include "GoogleMapWidget.generated.h"

//~~~~~ Forward Declaration ~~~~~
class AGoogleMapsGameMode;

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

	FLocationStruct(float lat, float lng, FDateTime time)
		: Latitude(lat),
		Longitude(lng),
		Time(time)
	{}
};


/**
 * 
 */
UCLASS()
class GOOGLEMAPS_API UGoogleMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Constructor declaration
	UGoogleMapWidget(const FObjectInitializer& ObjectInitializer);

	/** Remove the current menu widget and create a new one from the specified class, if provided. */
	UFUNCTION(BlueprintCallable, Category = "Google Maps")
	void DrawHistoricalPolyline();

	/** New GPS location has been received */
	UFUNCTION(BlueprintImplementableEvent, Category = "Google Maps")
	void OnLocationChanged(float Latitude, float Longitude);

	UFUNCTION(BlueprintCallable, Category = "Google Maps")
	void StartTracking();

	void LocationChanged(float lat, float lng, int64 time);
	FTimespan CalculateSplit(float over);
	void RecalculateTotalDistance();

	/** Should tracking be enabled for this map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	bool trackingEnabled;

	/** Has the Google API successfully established a GPS connection? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
		bool GPSConnected;

	/** Distance over which to calculate the split in km*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
		float SplitOverDistance;

	/** GPS data points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	TArray<FLocationStruct> GPSPoints;

	/** Tracking start time (UTC) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
		FDateTime StartTime;

	/** Total distance in km */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
		float TotalDistance;

	/** Time taken to run last 1 km */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
		FTimespan Split;

protected:
	// Overrides
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;


private:
	void CreateGoogleMap(FVector2D Position, FVector2D Size);
	void RemoveGoogleMap();
	void RunGPSService();
	void KillGPSService();

	void UpdateTotalDistance();
	float getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2);

	bool initialised;
};