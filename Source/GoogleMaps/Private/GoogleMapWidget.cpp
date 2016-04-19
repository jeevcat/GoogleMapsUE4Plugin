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
extern "C" void Java_com_epicgames_ue4_GameActivity_nativeLocationChanged(JNIEnv* jenv, jobject thiz, jdouble lat, jdouble lng)
{
	UE_LOG(LogGoogleMaps, Log, TEXT("nativeLocationChanged lat=%.4f and lng=%.4f"), lat, lng);
	// Send location to BP
	AttachedWidget->OnLocationChanged((float)lat, (float)lng);
}
#endif

