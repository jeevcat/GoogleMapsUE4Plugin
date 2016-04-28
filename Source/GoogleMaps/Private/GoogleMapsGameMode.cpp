// © 2016 Sam Jeeves. All Rights Reserved.

#include "GoogleMapsPrivatePCH.h"
#include "Kismet/GameplayStatics.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "GoogleMapsGameMode.h"

static bool shouldResumeTracking_static = false;
static AGoogleMapsGameMode* googleMapsGameMode_static;

AGoogleMapsGameMode::AGoogleMapsGameMode()
{
}

void AGoogleMapsGameMode::BeginPlay()
{
	ShouldResumeTracking = shouldResumeTracking_static;
	googleMapsGameMode_static = this;
	Super::BeginPlay();
}

// **** GoogleMaps native functions **** //

#if PLATFORM_ANDROID
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeResumeTracking(JNIEnv* jenv, jobject thiz)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeResumeTracking"));
	shouldResumeTracking_static = true;
}
#endif

#if PLATFORM_ANDROID
extern "C" void Java_com_jeevcatgames_UEMapDialog_nativeLocationChanged(JNIEnv* jenv, jobject thiz, jdouble lat, jdouble lng, jlong time)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeLocationChanged lat=%.4f, lng=%.4f, time:%d"), lat, lng, time);
	if(googleMapsGameMode_static->GoogleMapWidget != nullptr)
		googleMapsGameMode_static->GoogleMapWidget->LocationChanged(lat, lng, time);
}

extern "C" void Java_com_jeevcatgames_UEMapDialog_nativeAllPoints(JNIEnv* jenv, jobject thiz,
	jdoubleArray latArray, jdoubleArray lngArray, jlongArray timeArray)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeAllPoints"));

	if (googleMapsGameMode_static->GoogleMapWidget != nullptr) {
		UGoogleMapWidget* w = googleMapsGameMode_static->GoogleMapWidget;

		int pointCount = jenv->GetArrayLength(latArray);
		jdouble* lat = jenv->GetDoubleArrayElements(latArray, 0);
		jdouble* lng = jenv->GetDoubleArrayElements(lngArray, 0);
		jlong* time = jenv->GetLongArrayElements(timeArray, 0);
		for (int i = 0; i < pointCount; i++) {
			w->GPSPoints.Emplace(lat[i], lng[i], FDateTime::FromUnixTimestamp(time[i] / 1000));
		}

		// Set start time based on first GPS point. This will be a few seconds off
		w->StartTime = FDateTime::FromUnixTimestamp(time[0] / 1000);
		// Update total distance manually
		w->RecalculateTotalDistance();
		w->CalculateSplit(w->SplitOverDistance);
		w->GPSConnected = true;
	}
}

extern "C" void Java_com_jeevcatgames_UEMapDialog_nativeMapReady(JNIEnv* jenv, jobject thiz)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeMapReady"));
	if (googleMapsGameMode_static->GoogleMapWidget != nullptr)
		googleMapsGameMode_static->GoogleMapWidget->DrawHistoricalPolyline();
}
#endif