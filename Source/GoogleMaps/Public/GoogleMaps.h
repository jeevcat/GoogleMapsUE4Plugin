// Copyright 2016 Sam Jeeves. All Rights Reserved.
#pragma once
#include "GoogleMaps.generated.h"

UCLASS(MinimalAPI)
class UGoogleMapsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

private:
#if PLATFORM_ANDROID
	static void CallVoidMethodWithExceptionCheck(jmethodID Method, ...);
#endif
public:
	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	static void CreateGoogleMap(FVector2D Position, FVector2D Size);

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	static void RemoveGoogleMap();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	static void ConnectToGoogleAPI();

	UFUNCTION(BlueprintCallable, Category = "Utilities|Google Maps")
	static void DisconnectFromGoogleAPI();

	UFUNCTION(BlueprintImplementableEvent, Category = "Utilities|Google Maps")
	void OnLocationChanged(float Latitude, float Longitude);

};