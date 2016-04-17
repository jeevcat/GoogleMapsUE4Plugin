// Copyright 2016 Sam Jeeves. All Rights Reserved.
#pragma once
#include "GoogleMaps.generated.h"

UCLASS(MinimalAPI)
class UGoogleMapsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

private:
	// Nothing here

public:
	UFUNCTION(BlueprintCallable, Category = "Utilities|Platform")
		static void GoogleMapsExample(int32 adID);
};

#if PLATFORM_ANDROID
void AndroidThunkCpp_GoogleMapsExample(int32 adID);
#endif