// Copyright 2016 Sam Jeeves. All Rights Reserved.
#include "GoogleMapsPrivatePCH.h"
#include "GoogleMaps.h"

#if PLATFORM_ANDROID
#include "Runtime/Core/Public/CoreGlobals.h" //needed?
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DEFINE_LOG_CATEGORY(LogGoogleMaps);



// **** GoogleMaps BlueprintFunctionLibrary **** //

UGoogleMapsFunctionLibrary::UGoogleMapsFunctionLibrary(FObjectInitializer const&) {}

#if PLATFORM_ANDROID
static jmethodID AndroidThunkJava_CreateGoogleMap;
static jmethodID AndroidThunkJava_RemoveGoogleMap;
static jmethodID AndroidThunkJava_ConnectGoogleAPI;
static jmethodID AndroidThunkJava_DisconnectGoogleAPI;
#endif
#if PLATFORM_ANDROID

void UGoogleMapsFunctionLibrary::CallVoidMethodWithExceptionCheck(jmethodID Method, ...)
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

void UGoogleMapsFunctionLibrary::CreateGoogleMap(FVector2D Position, FVector2D Size)
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

void UGoogleMapsFunctionLibrary::RemoveGoogleMap()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_RemoveGoogleMap);
#endif
}

void UGoogleMapsFunctionLibrary::ConnectToGoogleAPI()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_ConnectGoogleAPI);
#endif
}

void UGoogleMapsFunctionLibrary::DisconnectFromGoogleAPI()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_DisconnectGoogleAPI);
#endif
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


// **** GoogleMaps native functions **** //

#if PLATFORM_ANDROID
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeLocationChanged(JNIEnv* jenv, jobject thiz, jdouble lat, jdouble lng)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeLocationChanged lat=%.4f and lng=%.4f"), lat, lng);

}
#endif