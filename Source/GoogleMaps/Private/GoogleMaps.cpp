// Copyright 2016 Sam Jeeves. All Rights Reserved.
#include "GoogleMapsPrivatePCH.h"
#include "GoogleMaps.h"

#if PLATFORM_ANDROID
#include "Runtime/Core/Public/CoreGlobals.h" //needed?
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DEFINE_LOG_CATEGORY(LogGoogleMaps);

UGoogleMapsFunctionLibrary::UGoogleMapsFunctionLibrary(FObjectInitializer const&) {}

#if PLATFORM_ANDROID
static jmethodID AndroidThunkJava_CreateGoogleMap;
static jmethodID AndroidThunkJava_RemoveGoogleMap;
#endif

void UGoogleMapsFunctionLibrary::CreateGoogleMap(FVector2D Position, FVector2D Size)
{
#if PLATFORM_ANDROID
	// call out to JNI to see if the application was packaged for GearVR
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (AndroidThunkJava_CreateGoogleMap)
		{
			jfloat resX = GSystemResolution.ResX;
			jfloat pX = Position.X;
			jfloat pY = Position.Y;
			jfloat sX = Size.X;
			jfloat sY = Size.Y;
			UE_LOG(LogGoogleMaps, Log, TEXT("Displaying GoogleMap at %.1f,%.1f, size:%.1f,%.1f"), pX, pY, sX, sY);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_CreateGoogleMap, pX, pY, sX, sY, resX);
		}
	}
#endif
}

void UGoogleMapsFunctionLibrary::RemoveGoogleMap()
{
#if PLATFORM_ANDROID
	// call out to JNI to see if the application was packaged for GearVR
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (AndroidThunkJava_RemoveGoogleMap)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, AndroidThunkJava_RemoveGoogleMap);
		}
	}
#endif
}



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
		}
	}
#endif
}


void FGoogleMaps::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}