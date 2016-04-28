// © 2016 Sam Jeeves. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "GoogleMapWidget.h"
#include "Misc/DateTime.h"

#include "GoogleMapsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GOOGLEMAPS_API AGoogleMapsGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGoogleMapsGameMode();
	
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Game was killed and is restarting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Google Maps")
	bool ShouldResumeTracking;

	/** Placeholder for the GoogleMapWidget when it is present in the game */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Google Maps")
	UGoogleMapWidget* GoogleMapWidget;
};
