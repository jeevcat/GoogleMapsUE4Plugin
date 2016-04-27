// © 2016 Sam Jeeves. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "GoogleMapWidget.h"

#include "GoogleMapsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GOOGLEMAPS_API AGoogleMapsGameMode : public AGameMode
{
	GENERATED_BODY()
	
	void UpdateTotalDistance();
	float getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2);

public:
	AGoogleMapsGameMode();

	/** Has the Google API successfully established a GPS connection? */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	bool GPSConnected;

	/** Distance over which to calculate the split in km*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	float SplitOverDistance;

	/** GPS data points */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	TArray<FLocationStruct> GPSPoints;

	/** Tracking start time (UTC) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	FDateTime startTime;

	/** Total distance in km */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	float TotalDistance;

	/** Time taken to run last 1 km */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	FTimespan Split;

	/** Game was killed and is restarting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	bool ShouldResumeTracking;

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	void RunGPSService();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	void StartTracking();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	void KillGPSService();

	/** New GPS location has been received */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Location Changed"))
	void OnLocationChanged(float Latitude, float Longitude);

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	void LocationChanged(float lat, float lng, int64 time);
	void UpdateSplit();
	void RecalculateTotalDistance();

protected:
	/** Placeholder for the GoogleMapWidget when it is present in the game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	UGoogleMapWidget* GoogleMapWidget;
};
