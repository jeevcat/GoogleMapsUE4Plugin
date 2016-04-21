// Copyright 2016 Sam Jeeves. All Rights Reserved.

#include "GoogleMapsPrivatePCH.h"
#include "GoogleMaps.h"
#include "GoogleMapWidget.h"
#include "GenericPlatform/GenericPlatformMath.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#if PLATFORM_ANDROID
static jmethodID AndroidThunkJava_CreateGoogleMap;
static jmethodID AndroidThunkJava_RemoveGoogleMap;
static jmethodID AndroidThunkJava_ConnectGoogleAPI;
static jmethodID AndroidThunkJava_DisconnectGoogleAPI;

void CallVoidMethodWithExceptionCheck(jmethodID Method, ...)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (Method)
		{
			va_list Args;
			va_start(Args, Method);
			Env->CallVoidMethodV(FJavaWrapper::GameActivityThis, Method, Args);
			va_end(Args);
		}
	}		
	if (FAndroidApplication::CheckJavaException())
		UE_LOG(LogGoogleMaps, Log, TEXT("Previous JNI call threw exception"));
}
#endif

float getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2) {
	float x = (lon2 - lon1)*(PI / 180.f) * FGenericPlatformMath::Cos((lat1 + lat2) * (PI / 180.f) / 2);
	float y = (lat2 - lat1)* (PI / 180.f);
	float R = 6371;
	return FGenericPlatformMath::Sqrt(x*x + y*y) * R;
}


// **** GoogleMaps Module Interface **** //

class FGoogleMaps : public IGoogleMaps
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FGoogleMaps, GoogleMaps)

void FGoogleMaps::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (FJavaWrapper::GameActivityClassID)
		{
			AndroidThunkJava_CreateGoogleMap = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_CreateGoogleMap", "(FFFFF)V", false);
			AndroidThunkJava_RemoveGoogleMap = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_RemoveGoogleMap", "()V", true);
			AndroidThunkJava_ConnectGoogleAPI = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_ConnectGoogleAPI", "()V", true);
			AndroidThunkJava_DisconnectGoogleAPI = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_DisconnectGoogleAPI", "()V", true);
		}
	}
#endif
}

void FGoogleMaps::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}