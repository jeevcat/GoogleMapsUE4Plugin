// Copyright 2016, Sam Jeeves. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
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
