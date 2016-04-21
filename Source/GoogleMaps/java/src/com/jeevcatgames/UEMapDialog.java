// Custom Dialog which holds a MapFragment for use in UnrealEngine
// Copyright 2016, Sam Jeeves. All rights reserved.

package com.jeevcatgames;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.IntentSender;
import android.location.Location;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.GoogleApiClient.ConnectionCallbacks;
import com.google.android.gms.common.api.GoogleApiClient.OnConnectionFailedListener;
import com.google.android.gms.common.api.PendingResult;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.LocationSettingsRequest;
import com.google.android.gms.location.LocationSettingsResult;
import com.google.android.gms.location.LocationSettingsStatusCodes;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.List;


public class UEMapDialog extends Dialog implements ConnectionCallbacks,
        OnConnectionFailedListener, LocationListener, ViewTreeObserver.OnGlobalLayoutListener,
        ResultCallback<LocationSettingsResult> {

    @SuppressWarnings("JniMissingFunction")
    public native void nativeLocationChanged(double lat, double lng);

    public boolean isShown;

    public UEMapDialog(final Context context) {
        super(context, android.R.style.Theme_Panel);
        parentActivity = (Activity) context;

        layoutId = parentActivity.getResources().getIdentifier("map_frame", "layout",
                parentActivity.getPackageName());
        mapContainerId = parentActivity.getResources().getIdentifier("map", "id",
                parentActivity.getPackageName());
        isShown = false;

        // Create GoogleMap Dialog
        inflater = (LayoutInflater)
                parentActivity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        FrameLayout frame = (FrameLayout) inflater.inflate(layoutId, null, false);
        setContentView(frame);

        // Create MapFragment
        mapFragment = (MapFragment) parentActivity.getFragmentManager()
                .findFragmentById(mapContainerId);

        mapFragment.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(GoogleMap map) {
                Log.i(TAG, "Google Map ready");
                googleMap = map;
                googleMap.setOnMyLocationButtonClickListener(
                        new GoogleMap.OnMyLocationButtonClickListener() {
                            @Override
                            public boolean onMyLocationButtonClick() {
                                followUser = true;
                                // Still centre map on location
                                return false;
                            }
                        });
            }
        });

        // Create an instance of GoogleAPIClient.
        apiClient = new GoogleApiClient.Builder(parentActivity)
            .addConnectionCallbacks(this)
            .addOnConnectionFailedListener(this)
            .addApi(LocationServices.API)
            .build();
    }

    private static final String TAG = "UEMapDialog";
    private static final int REQUEST_CHECK_SETTINGS = 0x1;

    private Activity parentActivity;
    private int layoutId, mapContainerId;
    private boolean followUser;
    private LayoutInflater inflater;
    private MapFragment mapFragment;
    private GoogleMap googleMap;
    private GoogleApiClient apiClient;
    private LocationRequest locationRequest;
    private Location lastLocation;
    private Polyline mapPolyline;


    @Override
    public void onConnected(Bundle bundle) {
        Log.i(TAG, "New LocationRequest");
        locationRequest = new LocationRequest()
            .setInterval(5000)
            .setFastestInterval(1000)
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
    public void onResult(LocationSettingsResult result) {
        final Status status = result.getStatus();
        //final LocationSettingsStates = result.getLocationSettingsStates();
        switch (status.getStatusCode()) {
            case LocationSettingsStatusCodes.SUCCESS:
                // All location settings are satisfied. The client can
                // initialize location requests here.
                Log.i(TAG, "All location settings are satisfied. Initialising location requests");
                startLocationUpdates();
                break;
            case LocationSettingsStatusCodes.RESOLUTION_REQUIRED:
                // by showing the user a dialog.
                Log.i(TAG, "Location settings are not satisfied, showing the user a dialog.");
                try {
                    // Show the dialog by calling startResolutionForResult(),
                    // and check the result in onActivityResult().
                    status.startResolutionForResult(
                            parentActivity,
                            REQUEST_CHECK_SETTINGS);
                } catch (IntentSender.SendIntentException e) {
                    // Ignore the error.
                }
                break;
            case LocationSettingsStatusCodes.SETTINGS_CHANGE_UNAVAILABLE:
                Log.i(TAG, "Location settings are not satisfied. Can't fix settings");
                // Location settings are not satisfied. However, we have no way
                // to fix the settings so we won't show the dialog.
                break;
        }
    }

    @Override
    public void onConnectionSuspended(int i) {
        Log.i(TAG, "onConnectionSuspended() called. Trying to reconnect.");
    }

    @Override
    public void onConnectionFailed(ConnectionResult result) {
        Log.i(TAG, "Google API connection failed: Has resolution? " + result.hasResolution());
    }

    @Override
    public void onLocationChanged(Location location) {
        if(location.getAccuracy() < 100.0f) {
            lastLocation = location;
            final LatLng newPoint = new LatLng(lastLocation.getLatitude(), lastLocation.getLongitude());
            parentActivity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (followUser) {
                        googleMap.animateCamera(CameraUpdateFactory.newLatLng(newPoint));
                    }
                    List<LatLng> points = mapPolyline.getPoints();
                    points.add(newPoint);
                    mapPolyline.setPoints(points);
                }
            });
            nativeLocationChanged(location.getLatitude(), location.getLongitude());
        } else {
            Log.i(TAG, "Low accuracy location (<100m). Requesting single fresh location");
            // Ask for single fresh location
            LocationRequest singleLR = new LocationRequest()
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY)
                .setNumUpdates(1)
                .setExpirationDuration(1000);
            LocationServices.FusedLocationApi.requestLocationUpdates(apiClient,
                    singleLR, this);
        }
    }

    @Override
    public void onGlobalLayout() {
        Log.i(TAG, "In OnResume OnGlobalLayoutListener");
        findViewById(android.R.id.content).getViewTreeObserver().removeOnGlobalLayoutListener(this);
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                show();
            }
        });
    }

    public void OnResume() {
        if (isShown) {
            Log.i(TAG, "Reshowing MapDialog");
            ViewTreeObserver vto = findViewById(android.R.id.content).getViewTreeObserver();
            vto.addOnGlobalLayoutListener(this);
        }
    }

    public void startLocationUpdates() {
        lastLocation = LocationServices.FusedLocationApi.getLastLocation(apiClient);
        LocationServices.FusedLocationApi.requestLocationUpdates(apiClient,
                locationRequest, this);
        googleMap.setMyLocationEnabled(true);
        // Instantiate a new Polyline object and add 1 point
        final PolylineOptions polyOptions = new PolylineOptions()
                .add(new LatLng(lastLocation.getLatitude(), lastLocation.getLongitude()));
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                googleMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
                        new LatLng(lastLocation.getLatitude(), lastLocation.getLongitude()), 18));
                mapPolyline = googleMap.addPolyline(polyOptions);
            }
        });
    }

    public void CreateGoogleMap(int posX, int posY, int sizeX, int sizeY) {
        // Move map
        final WindowManager.LayoutParams wmlp = getWindow().getAttributes();
        wmlp.gravity = Gravity.TOP | Gravity.LEFT;
        wmlp.x = posX;
        wmlp.y = posY;

        // Set map size
        mapFragment.getView().getLayoutParams().width = sizeX;
        mapFragment.getView().getLayoutParams().height = sizeY;

        isShown = true;
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                show();
            }
        });
    }

    public void RemoveGoogleMap() {
        isShown = false;
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.i(TAG, "Dismissing GoogleMap");
                dismiss();
            }
        });
    }

    public void ConnectGoogleAPI() {
        apiClient.connect();
    }

    public void DisconnectGoogleAPI() {
        if (apiClient != null)
            if (apiClient.isConnected()) {
                LocationServices.FusedLocationApi.removeLocationUpdates(
                        apiClient, this);
                apiClient.disconnect();
                parentActivity.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        googleMap.setMyLocationEnabled(false);
                    }
                });
            }
    }
}