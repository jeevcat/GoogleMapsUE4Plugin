// IRemoteService.aidl
package com.jeevcatgames;

// Declare any non-default types here with import statements

/** Example service interface */
interface IGPSService {
    /** Request the process ID of this service, to do evil things with it. */
    int getPid();

    /** Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat,
            double aDouble, String aString);
}