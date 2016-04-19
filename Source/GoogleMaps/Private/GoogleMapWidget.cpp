// Copyright 2016, Sam Jeeves. All rights reserved.

#include "GoogleMapsPrivatePCH.h"
#include "GoogleMapWidget.h"
#include "GoogleMaps.h"
#if PLATFORM_ANDROID
#include "Runtime/Core/Public/CoreGlobals.h"
#endif

DEFINE_LOG_CATEGORY(LogGoogleMaps);

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
#endif
}

void UGoogleMapWidget::DisconnectFromGoogleAPI()
{
#if PLATFORM_ANDROID
	CallVoidMethodWithExceptionCheck(AndroidThunkJava_DisconnectGoogleAPI);
#endif
}


