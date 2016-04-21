// Copyright 2016, Sam Jeeves. All rights reserved.

#include "GoogleMapsPrivatePCH.h"
#include "GoogleMapWidget.h"
#include "GoogleMaps.h"

#include "SlateBlueprintLibrary.h"
#if PLATFORM_ANDROID
#include "Runtime/Core/Public/CoreGlobals.h"
#endif

DEFINE_LOG_CATEGORY(LogGoogleMaps);

static UGoogleMapWidget* AttachedWidget = NULL;

UGoogleMapWidget::UGoogleMapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), GPSConnected(false), TotalDistance(0), Split(0), StartTime(0)
{}

void UGoogleMapWidget::LocationChanged(float lat, float lng)
{	//First time this is run
	if (!GPSConnected) {
		GPSConnected = true;
		StartTime = FDateTime::Now();
	}

	GPSPoints.Emplace(lat, lng);

	if (GPSPoints.Num() > 1) {
		UpdateSplit(0.2f);
		TotalDistance += getDistanceFromLatLonInKm(GPSPoints.Last(1).Latitude,
			GPSPoints.Last(1).Longitude,
			lat,
			lng);
	}

	// Send location to BP
	OnLocationChanged((float)lat, (float)lng);
}

//Override
void UGoogleMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Do once
	if (!initialised) {
		initialised = true;
		FVector2D pixel;
		FVector2D viewport;
		USlateBlueprintLibrary::LocalToViewport(this, MyGeometry, FVector2D(0, 0), pixel, viewport);
		FVector2D size = USlateBlueprintLibrary::GetLocalSize(MyGeometry);
		CreateGoogleMap(pixel, size);
	}

}

//Override
void UGoogleMapWidget::NativeDestruct()
{
	RemoveGoogleMap();
	Super::NativeDestruct();
}


void UGoogleMapWidget::CreateGoogleMap(FVector2D Position, FVector2D Size)
{
#if PLATFORM_ANDROID
	jfloat resX = GSystemResolution.ResX;
	jfloat pX = Position.X;
	jfloat pY = Position.Y;
	jfloat sX = Size.X;
	jfloat sY = Size.Y;
	UE_LOG(LogGoogleMaps, Log, TEXT("Displaying GoogleMap at %.1f,%.1f, size:%.1f,%.1f"), pX, pY, sX, sY);
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_CreateGoogleMap, pX, pY, sX, sY, resX);
#endif
}

void UGoogleMapWidget::RemoveGoogleMap()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_RemoveGoogleMap);
#endif
}

void UGoogleMapWidget::UpdateSplit(float overDistance)
{
	float d = 0;
	int i = GPSPoints.Num() - 1;
	while (d < overDistance)
	{
		d += getDistanceFromLatLonInKm(GPSPoints[i].Latitude, GPSPoints[i].Longitude,
			GPSPoints[i-1].Latitude, GPSPoints[i - 1].Longitude);
		i--;
		
		if (i == 0) { // Distance isn't overDistance yet
			Split = FTimespan(0);
			return;
		}
	}
	Split = (GPSPoints.Last(0).Time - GPSPoints.Last(i).Time) * (1/d);

}

void UGoogleMapWidget::ConnectToGoogleAPI()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_ConnectGoogleAPI);
	AttachedWidget = this;
#endif
}

void UGoogleMapWidget::DisconnectFromGoogleAPI()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_DisconnectGoogleAPI);
	AttachedWidget = NULL;
#endif
}

// **** GoogleMaps native functions **** //
#if PLATFORM_ANDROID
extern "C" void Java_com_jeevcatgames_UEMapDialog_nativeLocationChanged(JNIEnv* jenv, jobject thiz, jdouble lat, jdouble lng)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeLocationChanged lat=%.4f and lng=%.4f"), lat, lng);
	AttachedWidget->LocationChanged(lat, lng);
}
#endif