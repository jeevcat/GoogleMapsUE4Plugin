// Copyright 2016 Sam Jeeves. All Rights Reserved.
#include "GoogleMapsPrivatePCH.h"
#include "GoogleMaps.h"

#if PLATFORM_ANDROID
#include "Runtime/Core/Public/CoreGlobals.h" //needed?
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DEFINE_LOG_CATEGORY(LogAdBox);

UGoogleMapsFunctionLibrary::UGoogleMapsFunctionLibrary(FObjectInitializer const&) {}

void UGoogleMapsFunctionLibrary::GoogleMapsExample(int32 adID)
{
#if PLATFORM_ANDROID
	AndroidThunkCpp_GoogleMapsExample(adID);
#endif
}


#if PLATFORM_ANDROID
static jmethodID AndroidThunkJava_GoogleMapsExample;

// call out to JNI to see if the application was packaged for GearVR
void AndroidThunkCpp_GoogleMapsExample(int32 adID)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		if (AndroidThunkJava_GoogleMapsExample)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GoogleServicesThis, AndroidThunkJava_GoogleMapsExample, adID);
		}
	}
}
#endif


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
			AndroidThunkJava_GoogleMapsExample = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_GoogleMapsExample", "(Ljava/lang/Integer;)V", true);
		}
	}
#endif
}


void FGoogleMaps::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}