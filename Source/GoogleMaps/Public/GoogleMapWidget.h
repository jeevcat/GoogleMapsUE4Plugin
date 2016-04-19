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
	
public:
	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
		void CreateGoogleMap(FVector2D Position, FVector2D Size);

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
		void RemoveGoogleMap();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
		void ConnectToGoogleAPI();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
		void DisconnectFromGoogleAPI();

	UFUNCTION(BlueprintImplementableEvent, Category = "Utilities|Google Maps")
		void OnLocationChanged(float Latitude, float Longitude);
	
	
};
