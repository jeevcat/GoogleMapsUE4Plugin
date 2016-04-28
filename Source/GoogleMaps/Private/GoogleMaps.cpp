// Copyright 2016 Sam Jeeves. All Rights Reserved.

#include "GoogleMapsPrivatePCH.h"
#include "GoogleMaps.h"


#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

#if PLATFORM_ANDROID
static jmethodID AndroidThunkJava_CreateGoogleMap;
static jmethodID AndroidThunkJava_RemoveGoogleMap;
static jmethodID AndroidThunkJava_RunGPSService;
static jmethodID AndroidThunkJava_KillGPSService;
static jmethodID AndroidThunkJava_StartTracking;
static jmethodID AndroidThunkJava_DrawHistoricalPolyline;

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
			AndroidThunkJava_CreateGoogleMap = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_CreateGoogleMap", "(ZFFFFF)V", false);
			AndroidThunkJava_RemoveGoogleMap = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_RemoveGoogleMap", "()V", true);
			AndroidThunkJava_RunGPSService = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_RunGPSService", "()V", true);
			AndroidThunkJava_KillGPSService = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_KillGPSService", "()V", true);
			AndroidThunkJava_StartTracking = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_StartTracking", "()V", true);
			AndroidThunkJava_DrawHistoricalPolyline = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_DrawHistoricalPolyline", "([F[F)V", false);
		}
	}
#endif
}

void FGoogleMaps::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}