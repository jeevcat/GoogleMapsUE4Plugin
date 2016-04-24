package com.jeevcatgames;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.Location;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.epicgames.ue4.GameActivity;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.PendingResult;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.LocationSettingsRequest;
import com.google.android.gms.location.LocationSettingsResult;
import com.google.android.gms.location.LocationSettingsStatusCodes;
import com.google.android.gms.maps.model.LatLng;

import java.io.Serializable;
import java.util.ArrayList;


/**
 * Custom Service which will continually pull GPS positions until stopped.
 * Copyright 2016, Sam Jeeves. All rights reserved.
 */


public class GPSService extends Service implements GoogleApiClient.ConnectionCallbacks,
        ResultCallback<LocationSettingsResult>, LocationListener, GoogleApiClient.OnConnectionFailedListener {
    public static final String CONNECT_TO_GOOGLE_API = "com.jeevcatgames.GPSService.CONNECT_TO_GOOGLE_API";
    public static final String START_TRACKING = "com.jeevcatgames.GPSService.START_TRACKING";
    public static final String REQUEST_ALL_POINTS = "com.jeevcatgames.GPSService.REQUEST_ALL_POINTS";
    public static final String LOCATION_RECIEVED = "com.jeevcatgames.GPSService.LOCATION_RECIEVED";

    private NotificationManager mNM;
    private GoogleApiClient apiClient;
    private LocationRequest locationRequest;
    private BroadcastReceiver receiver;
    private ArrayList<LatLngTime> allPoints;

    private static final String TAG = "GPSService";
    private static final int LOCATION_INTERVAL = 5000;
    private static final int FASTEST_INTERVAL = 1000;
    private int NOTIFICATION = 1234;
    private boolean gameIsAlive = true;


    @Override
    public void onCreate() {
        super.onCreate();
        mNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

        // Display a notification about us starting.  We put an icon in the status bar.
        showNotification();

        // Array of all lat, lng and times
        allPoints = new ArrayList<LatLngTime>();

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(CONNECT_TO_GOOGLE_API);
        intentFilter.addAction(START_TRACKING);
        intentFilter.addAction(REQUEST_ALL_POINTS);
        intentFilter.addAction(LOCATION_RECIEVED);

        receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (intent.getAction().equals(CONNECT_TO_GOOGLE_API)) {
                    ConnectToGoogleAPI();
                }
                if (intent.getAction().equals(START_TRACKING)) {
                    StartTracking();
                }
                if (intent.getAction().equals(REQUEST_ALL_POINTS)) {
                    RequestAllPoints();
                }
                if (intent.getAction().equals(LOCATION_RECIEVED)) {
                    gameIsAlive = true;
                }
            }
        };
        registerReceiver(receiver, intentFilter);

        // Create an instance of GoogleAPIClient.
        apiClient = new GoogleApiClient.Builder(this)
                .addConnectionCallbacks(this)
                .addOnConnectionFailedListener(this)
                .addApi(LocationServices.API)
                .build();
    }



    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Toast.makeText(this, "service starting", Toast.LENGTH_SHORT).show();
        // If we get killed, after returning from here, restart
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        // Cancel the persistent notification.
        mNM.cancel(NOTIFICATION);

        // Tell the user we stopped.
        Toast.makeText(this, "Service stopped", Toast.LENGTH_SHORT).show();

        if (apiClient != null)
            if (apiClient.isConnected()) {
                LocationServices.FusedLocationApi.removeLocationUpdates(
                        apiClient, this);
                apiClient.disconnect();
            }

        unregisterReceiver(receiver);
    }

    @Override
    public IBinder onBind(Intent intent) {
        // We don't provide binding, so return null
        return null;
    }

    /**
     * Show a notification while this service is running.
     */
    private void showNotification() {
        // In this sample, we'll use the same text for the ticker and the expanded notification
        CharSequence text = "Whats up";

        // The PendingIntent to launch our activity if the user selects this notification
        Intent intent = new Intent(getApplicationContext(), GameActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);

        PendingIntent contentIntent = PendingIntent.getActivity(this, NOTIFICATION,
                intent, PendingIntent.FLAG_UPDATE_CURRENT);

        // Set the info for the views that show in the notification panel.
        Notification notification = new Notification.Builder(this)
                .setTicker(text)  // the status text
                .setSmallIcon(android.R.drawable.sym_def_app_icon)
                .setWhen(System.currentTimeMillis())  // the time stamp
                .setContentTitle(text)  // the label of the entry
                .setContentText(text)  // the contents of the entry
                .setContentIntent(contentIntent)  // The intent to send when the entry is clicked
                .setOngoing(true)
                .build();

        // Send the notification.
        mNM.notify(NOTIFICATION, notification);
    }

    @Override
    public void onConnected(Bundle bundle) {
        Log.i(TAG, "New LocationRequest");
        locationRequest = new LocationRequest()
                .setInterval(LOCATION_INTERVAL)
                .setFastestInterval(FASTEST_INTERVAL)
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

        Log.i(TAG, "Checking for GPS settings");
        LocationSettingsRequest.Builder builder = new LocationSettingsRequest.Builder()
                .addLocationRequest(locationRequest);
        PendingResult<LocationSettingsResult> result =
                LocationServices.SettingsApi.checkLocationSettings(apiClient,
                        builder.build());
        result.setResultCallback(this);
    }

    @Override
    public void onConnectionSuspended(int i) {
        Log.i(TAG, "onConnectionSuspended() called. Trying to reconnect.");
    }

    @Override
    public void onResult(LocationSettingsResult result) {
        final Status status = result.getStatus();
        //final LocationSettingsStates = result.getLocationSettingsStates();
        Intent intent;
        switch (status.getStatusCode()) {
            case LocationSettingsStatusCodes.SUCCESS:
                // All location settings are satisfied. The client can
                // initialize location requests here.
                Log.i(TAG, "All location settings are satisfied. Initialising location requests");
                intent = new Intent(UEMapDialog.START_LOCATION_UPDATES);
                sendBroadcast(intent);
                break;
            case LocationSettingsStatusCodes.RESOLUTION_REQUIRED:
                // by showing the user a dialog.
                Log.i(TAG, "Location settings are not satisfied, showing the user a dialog.");
                intent = new Intent(UEMapDialog.USER_REQUEST_GPS_SETTING);
                intent.putExtra("Status", status);
                sendBroadcast(intent);
                break;
            case LocationSettingsStatusCodes.SETTINGS_CHANGE_UNAVAILABLE:
                Log.i(TAG, "Location settings are not satisfied. Can't fix settings");
                // Location settings are not satisfied. However, we have no way
                // to fix the settings so we won't show the dialog.
                break;
        }
    }

    @Override
    public void onLocationChanged(Location loc) {
        Log.i(TAG,"GAME IS ALIVE: "+gameIsAlive);

        if (loc.getAccuracy() < 100.0f) {
            LatLngTime newPoint = new LatLngTime(loc);
            allPoints.add(newPoint);
            if(gameIsAlive) {
                Intent intent = new Intent(UEMapDialog.UPDATE_MAP);
                intent.putExtra("LatLngTime", newPoint);
                sendBroadcast(intent);
                gameIsAlive = false;
            }
        } else {
            Log.i(TAG, "Low accuracy location (>100m). Requesting single fresh location");
            // Ask for single fresh location
            LocationRequest singleLR = new LocationRequest()
                    .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY)
                    .setNumUpdates(1)
                    .setExpirationDuration(1000);
            LocationServices.FusedLocationApi.requestLocationUpdates(apiClient,
                    singleLR, this);
        }
    }

    private void ConnectToGoogleAPI() {
        apiClient.connect();
    }

    private void StartTracking() {
        allPoints.add(new LatLngTime(LocationServices.FusedLocationApi.getLastLocation(apiClient)));
        LocationServices.FusedLocationApi.requestLocationUpdates(apiClient,
                locationRequest, this);

    }

    private void RequestAllPoints() {
        gameIsAlive = true;
        ArrayList<LatLngTime> points = new ArrayList<LatLngTime>();

        Intent intent = new Intent(UEMapDialog.RECEIVE_ALL_POINTS);
        intent.putExtra("Points", points);
        sendBroadcast(intent);
    }

    @Override
    public void onConnectionFailed(ConnectionResult result) {
        Log.i(TAG, "Google API connection failed: Has resolution? " + result.hasResolution());
    }

    static class LatLngTime implements Serializable {
        public double Latitude;
        public double Longitude;
        public long UTCTime;

        public LatLngTime(double latitude, double longitude, long utcTime) {
            Latitude = latitude;
            Longitude = longitude;
            UTCTime = utcTime;
        }

        public LatLngTime(Location location) {
            Latitude = location.getLatitude();
            Longitude = location.getLongitude();
            UTCTime = location.getTime();
        }

        public LatLng toLatLng() {
            return new LatLng(Latitude, Longitude);
        }
    }
}