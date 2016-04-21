// Copyright 2016 Sam Jeeves. All Rights Reserved.
#pragma once

#if PLATFORM_ANDROID
void CallVoidMethodWithExceptionCheck(jmethodID Method, ...);
#endif

float getDistanceFromLatLonInKm(float lat1, float lon1, float lat2, float lon2);