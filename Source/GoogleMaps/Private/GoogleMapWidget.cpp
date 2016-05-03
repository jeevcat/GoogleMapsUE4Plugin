// Copyright 2016, Sam Jeeves. All rights reserved.

#include "GoogleMapsPrivatePCH.h"
#include "GoogleMapWidget.h"
#include "GoogleMaps.h"

#include "SlateBlueprintLibrary.h"
#if PLATFORM_ANDROID
#include "Runtime/Core/Public/CoreGlobals.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DEFINE_LOG_CATEGORY(LogGoogleMaps);

UGoogleMapWidget::UGoogleMapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), GPSConnected(false), SplitOverDistance(0.2f) {}

void UGoogleMapWidget::DrawHistoricalPolyline()
{
#if PLATFORM_ANDROID
	if(!trackingEnabled)
		if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
		{
			int size = GPSPoints.Num();
			UE_LOG(LogGoogleMaps, Log, TEXT("DrawHistoricalPolyline, array size: %d"), size);
			jfloat* lat = new jfloat[size];
			jfloat* lng = new jfloat[size];
			for (int i = 0; i < size; i++) {
				lat[i] = GPSPoints[i].Latitude;
				lng[i] = GPSPoints[i].Longitude;
			}
			jfloatArray jlat,  jlng;
			jlat = Env->NewFloatArray(size);
			jlng = Env->NewFloatArray(size);
			if (jlat == NULL || jlng == NULL) {
				return; /* out of memory error thrown */
			}
			Env->SetFloatArrayRegion(jlat, 0, size, lat);
			Env->SetFloatArrayRegion(jlng, 0, size, lng);

			CallVoidMethodWithExceptionCheck(AndroidThunkJava_DrawHistoricalPolyline, jlat, jlng);

			delete[] lat;
			delete[] lng;
		}
#endif
}

void UGoogleMapWidget::CreateGoogleMap(FVector2D Position, FVector2D Size)
{
#if PLATFORM_ANDROID
	jfloat resX = GSystemResolution.ResX;
	jfloat pX = Position.X;
	jfloat pY = Position.Y;
	jfloat sX = Size.X;
	jfloat sY = Size.Y;
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_CreateGoogleMap, trackingEnabled, pX, pY, sX, sY, resX);
#endif
}

void UGoogleMapWidget::RemoveGoogleMap()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_RemoveGoogleMap);
#endif
}

FTimespan UGoogleMapWidget::CalculateSplit(float over)
{
	float d = 0;
	int i = GPSPoints.Num() - 1;
	while (d <= over && i > 0)
	{
		d += getDistanceFromLatLonInKm(GPSPoints[i].Latitude, GPSPoints[i].Longitude,
			GPSPoints[i - 1].Latitude, GPSPoints[i - 1].Longitude);
		i--;
	}
	if (d > 0)
		return (GPSPoints.Top().Time - GPSPoints[i].Time) * (1 / d);
	else
		return FTimespan(0);
}

void UGoogleMapWidget::UpdateTotalDistance()
{
	TotalDistance += getDistanceFromLatLonInKm(
		GPSPoints.Last(1).Latitude,
		GPSPoints.Last(1).Longitude,
		GPSPoints.Last(0).Latitude,
		GPSPoints.Last(0).Longitude);
}

void UGoogleMapWidget::RecalculateTotalDistance()
{
	if (GPSPoints.Num() > 1)
	{
		for (int i = 0; i < GPSPoints.Num() - 2; i++) {
			TotalDistance += getDistanceFromLatLonInKm(
				GPSPoints.Last(i + 1).Latitude,
				GPSPoints.Last(i + 1).Longitude,
				GPSPoints.Last(i).Latitude,
				GPSPoints.Last(i).Longitude);
		}
	}
}


//Privates

void UGoogleMapWidget::LocationChanged(float lat, float lng, int64 time)
{
	FDateTime dateTime = FDateTime::FromUnixTimestamp(time / 1000);

	GPSPoints.Emplace(lat, lng, dateTime);

	if (GPSPoints.Num() > 1) {
		Split = CalculateSplit(SplitOverDistance);
		UpdateTotalDistance();
	}

	// Send location to BP
	OnLocationChanged((float)lat, (float)lng);
}

float UGoogleMapWidget::getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2) {
	float x = (lon2 - lon1)*(PI / 180.f) * FGenericPlatformMath::Cos((lat1 + lat2) * (PI / 180.f) / 2);
	float y = (lat2 - lat1)* (PI / 180.f);
	float R = 6371; // Radius of the earth
	return FGenericPlatformMath::Sqrt(x*x + y*y) * R;
}

//Override
void UGoogleMapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Do once
	if (!initialised) {
		initialised = true;
		FVector2D pixel, viewport, size;
		USlateBlueprintLibrary::LocalToViewport(this, MyGeometry, FVector2D(0, 0), pixel, viewport);
		USlateBlueprintLibrary::LocalToViewport(this, MyGeometry, USlateBlueprintLibrary::GetLocalSize(MyGeometry), size, viewport);
		UE_LOG(LogGoogleMaps, Log, TEXT("Displaying GoogleMap at %.1f,%.1f, size:%.1f,%.1f"), pixel.X, pixel.Y, size.X, size.Y);
		CreateGoogleMap(pixel, size);
	}

}

//Override
void UGoogleMapWidget::NativeDestruct()
{
	RemoveGoogleMap();
	if (trackingEnabled)
		KillGPSService();
	((AGoogleMapsGameMode*)GetWorld()->GetAuthGameMode())->GoogleMapWidget = nullptr;
	Super::NativeDestruct();
}

//Override
void UGoogleMapWidget::NativeConstruct()
{
	((AGoogleMapsGameMode*)GetWorld()->GetAuthGameMode())->GoogleMapWidget = this;
	if (trackingEnabled)
		RunGPSService();
}

// **** GoogleMaps Java functions **** //
void UGoogleMapWidget::RunGPSService()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_RunGPSService);
#endif
}

void UGoogleMapWidget::StartTracking()
{
	if (trackingEnabled)
	{
		GPSConnected = true;
		StartTime = FDateTime::UtcNow();
#if PLATFORM_ANDROID
		CallVoidMethodWithExceptionCheck(AndroidThunkJava_StartTracking);
#endif
	}
}

void UGoogleMapWidget::KillGPSService()
{
	GPSConnected = false;
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_KillGPSService);
#endif
}


