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
	
public:
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

	/** Game was killed and is restarting */
	UFUNCTION(BlueprintPure, Category = "Utilities|Google Maps")
	bool ShouldResumeTracking() const;

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	void ConnectToGoogleAPI();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	void DisconnectFromGoogleAPI();

	/** New GPS location has been received */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Location Changed"))
	void OnLocationChanged(float Latitude, float Longitude);

	/** Called when the game starts. */
	virtual void BeginPlay() override;

	void LocationChanged(float lat, float lng);


protected:
	/** Placeholder for the GoogleMapWidget when it is present in the game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	UGoogleMapWidget* GoogleMapWidget;

private:
	void UpdateSplit(float overDistance);
	float getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2);
};
