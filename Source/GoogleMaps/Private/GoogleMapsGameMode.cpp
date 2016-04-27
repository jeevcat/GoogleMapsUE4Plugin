// © 2016 Sam Jeeves. All Rights Reserved.

#include "GoogleMapsPrivatePCH.h"
#include "GoogleMapsGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GenericPlatform/GenericPlatformMath.h"

static AGoogleMapsGameMode* GoogleMapsGameMode_c = NULL;
static bool shouldResumeTracking_static = false;

AGoogleMapsGameMode::AGoogleMapsGameMode()
{
	GPSConnected = false;
	SplitOverDistance = 0.2f;
}

void AGoogleMapsGameMode::BeginPlay()
{
	GoogleMapsGameMode_c = this;
	//if (shouldResumeTracking_static) GPSConnected = true;
	ShouldResumeTracking = shouldResumeTracking_static;
	Super::BeginPlay();
}

void AGoogleMapsGameMode::UpdateSplit()
{
	float d = 0;
	int i = GPSPoints.Num() - 1;
	while (d < SplitOverDistance && i > 0)
	{
		d += getDistanceFromLatLonInKm(GPSPoints[i].Latitude, GPSPoints[i].Longitude,
			GPSPoints[i - 1].Latitude, GPSPoints[i - 1].Longitude);
		i--;
	}
	if (d > 0)
		Split = (GPSPoints.Top().Time - GPSPoints[i].Time) * (1 / d);
}

void AGoogleMapsGameMode::UpdateTotalDistance()
{
	TotalDistance += getDistanceFromLatLonInKm(
		GPSPoints.Last(1).Latitude,
		GPSPoints.Last(1).Longitude,
		GPSPoints.Last(0).Latitude,
		GPSPoints.Last(0).Longitude);
}

void AGoogleMapsGameMode::RecalculateTotalDistance()
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

void AGoogleMapsGameMode::RunGPSService()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_RunGPSService);
#endif
}

void AGoogleMapsGameMode::StartTracking()
{
	GPSConnected = true;
	StartTime = FDateTime::UtcNow();
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_StartTracking);
#endif
}

void AGoogleMapsGameMode::KillGPSService()
{
	GPSConnected = false;
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_KillGPSService);
#endif
}


//Privates

void AGoogleMapsGameMode::LocationChanged(float lat, float lng, int64 time)
{
	FDateTime dateTime = FDateTime::FromUnixTimestamp(time / 1000);

	GPSPoints.Emplace(lat, lng, dateTime);

	if (GPSPoints.Num() > 1) {
		UpdateSplit();
		UpdateTotalDistance();
	}

	// Send location to BP
	OnLocationChanged((float)lat, (float)lng);
}

float AGoogleMapsGameMode::getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2) {
	float x = (lon2 - lon1)*(PI / 180.f) * FGenericPlatformMath::Cos((lat1 + lat2) * (PI / 180.f) / 2);
	float y = (lat2 - lat1)* (PI / 180.f);
	float R = 6371; // Radius of the earth
	return FGenericPlatformMath::Sqrt(x*x + y*y) * R;
}


// **** GoogleMaps native functions **** //
#if PLATFORM_ANDROID
extern "C" void Java_com_jeevcatgames_UEMapDialog_nativeLocationChanged(JNIEnv* jenv, jobject thiz, jdouble lat, jdouble lng, jlong time)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeLocationChanged lat=%.4f, lng=%.4f, time:%d"), lat, lng, time);
	GoogleMapsGameMode_c->LocationChanged(lat, lng, time);
}

extern "C" void Java_com_jeevcatgames_UEMapDialog_nativeAllPoints(JNIEnv* jenv, jobject thiz,
	jdoubleArray latArray, jdoubleArray lngArray, jlongArray timeArray)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeAllPoints"));

	int pointCount = jenv->GetArrayLength(latArray);
	jdouble* lat = jenv->GetDoubleArrayElements(latArray, 0);
	jdouble* lng = jenv->GetDoubleArrayElements(lngArray, 0);
	jlong* time = jenv->GetLongArrayElements(timeArray, 0);
	for (int i = 0; i < pointCount; i++) {
		GoogleMapsGameMode_c->GPSPoints.Emplace(lat[i], lng[i], FDateTime::FromUnixTimestamp(time[i] / 1000));
	}

	// Set start time based on first GPS point. This will be a few seconds off
	GoogleMapsGameMode_c->StartTime = FDateTime::FromUnixTimestamp(time[0] / 1000);
	// Update total distance manually
	GoogleMapsGameMode_c->RecalculateTotalDistance();
	GoogleMapsGameMode_c->UpdateSplit();
	GoogleMapsGameMode_c->GPSConnected = true;
}

extern "C" void Java_com_epicgames_ue4_GameActivity_nativeResumeTracking(JNIEnv* jenv, jobject thiz)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeResumeTracking"));
	shouldResumeTracking_static = true;
}
#endif