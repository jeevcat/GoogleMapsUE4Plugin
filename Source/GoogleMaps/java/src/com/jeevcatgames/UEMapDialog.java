package com.jeevcatgames;

/**
 * Custom Dialog which holds a MapFragment for use in UnrealEngine
 * Copyright 2016, Sam Jeeves. All rights reserved.
 */

import android.app.Activity;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.IntentSender;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.InflateException;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;

import com.google.android.gms.common.api.Status;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;
import java.util.List;


public class UEMapDialog extends Dialog implements ViewTreeObserver.OnGlobalLayoutListener, OnMapReadyCallback {

    public static final String USER_REQUEST_GPS_SETTING = "com.jeevcatgames.UEMapDialog.USER_REQUEST_GPS_SETTING";
    public static final String START_LOCATION_UPDATES = "com.jeevcatgames.UEMapDialog.START_LOCATION_UPDATES";
    public static final String UPDATE_MAP = "com.jeevcatgames.UEMapDialog.UPDATE_MAP";
    public static final String RECEIVE_ALL_POINTS = "com.jeevcatgames.UEMapDialog.RECEIVE_ALL_POINTS";
    private static final String TAG = "UEMapDialog";
    private static final int REQUEST_CHECK_SETTINGS = 0x1;
    private int posX, posY, sizeX, sizeY;
    private Activity parentActivity;
    private View frame;
    private int layoutId, mapContainerId;
    private boolean followUser, serviceAlreadyRunning;
    private LayoutInflater inflater;
    private MapFragment mapFragment;
    private GoogleMap googleMap;
    private Polyline mapPolyline;
    private BroadcastReceiver receiver;


    // Constructor
    public UEMapDialog(final Context context, boolean serviceAlreadyRunning, int pX, int pY, int sX, int sY) {
        super(context, android.R.style.Theme_Panel);
        parentActivity = (Activity) context;
        followUser = true;

        this.posX = pX;
        this.posY = pY;
        this.sizeX = sX;
        this.sizeY = sY;
        this.serviceAlreadyRunning = serviceAlreadyRunning;
    }

    @SuppressWarnings("JniMissingFunction")
    public native void nativeLocationChanged(double lat, double lng);

    /**
     * Overrides
     */

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        layoutId = parentActivity.getResources().getIdentifier("map_frame", "layout",
                parentActivity.getPackageName());
        mapContainerId = parentActivity.getResources().getIdentifier("map", "id",
                parentActivity.getPackageName());


        // Create GoogleMap Dialog
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        frame = parentActivity.getLayoutInflater().inflate(layoutId, null);
        setContentView(frame);
        setCancelable(false);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL);

        // Create MapFragment
        mapFragment = (MapFragment) parentActivity.getFragmentManager()
                .findFragmentById(mapContainerId);

        mapFragment.getMapAsync(this);

        // Move map
        WindowManager.LayoutParams wmlp = getWindow().getAttributes();
        wmlp.gravity = Gravity.TOP | Gravity.LEFT;
        wmlp.x = posX;
        wmlp.y = posY;

        // Set map size
        mapFragment.getView().getLayoutParams().width = sizeX;
        mapFragment.getView().getLayoutParams().height = sizeY;

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(USER_REQUEST_GPS_SETTING);
        intentFilter.addAction(START_LOCATION_UPDATES);
        intentFilter.addAction(UPDATE_MAP);

        receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if (intent.getAction().equals(USER_REQUEST_GPS_SETTING))
                    RequestGPSEnabled((Status) intent.getParcelableExtra("Status"));
                if (intent.getAction().equals(START_LOCATION_UPDATES))
                    StartLocationUpdates();
                if (intent.getAction().equals(UPDATE_MAP))
                    UpdateMap((LatLng) intent.getParcelableExtra("LatLng"));
                if (intent.getAction().equals(RECEIVE_ALL_POINTS))
                    ReceiveAllPoints((ArrayList<LatLng>) intent.getSerializableExtra("Points"));
            }
        };

        parentActivity.registerReceiver(receiver, intentFilter);

        Log.i(TAG, "ServiceAlreadyRunning: " + serviceAlreadyRunning);
        if(serviceAlreadyRunning) {
            Intent intent = new Intent(GPSService.REQUEST_ALL_POINTS);
            parentActivity.sendBroadcast(intent);
        } else {
            Intent intent = new Intent(GPSService.CONNECT_TO_GOOGLE_API);
            parentActivity.sendBroadcast(intent);
        }
    }

    @Override
    protected void onStop() {
        ViewGroup parent = (ViewGroup) frame.getParent();
            if (parent != null)
                parent.removeView(frame);

        if (mapFragment != null)
            parentActivity.getFragmentManager().beginTransaction().remove(mapFragment).commitAllowingStateLoss();

        if (receiver != null) {
            parentActivity.unregisterReceiver(receiver);
            receiver = null;
        }
        Log.i(TAG, "UEMapDialog is going down!");
        super.onStop();
    }

    @Override
    public void onMapReady(GoogleMap map) {
        Log.i(TAG, "Google Map ready");
        googleMap = map;
        map.setMyLocationEnabled(true);
        map.animateCamera(CameraUpdateFactory.zoomTo(18));
        PolylineOptions polyOptions = new PolylineOptions();
        mapPolyline = map.addPolyline(polyOptions);
        googleMap.setOnMyLocationButtonClickListener(
                new GoogleMap.OnMyLocationButtonClickListener() {
                    @Override
                    public boolean onMyLocationButtonClick() {
                        followUser = true;
                        return false;
                    }
                });
    }


    @Override
    public void onGlobalLayout() {
        Log.i(TAG, "In OnResume OnGlobalLayoutListener");
        parentActivity.findViewById(android.R.id.content).getViewTreeObserver().removeOnGlobalLayoutListener(this);
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                show();
            }
        });
    }

    public void OnResume() {
        Log.i(TAG, "Reshowing MapDialog");
        ViewTreeObserver vto = parentActivity.findViewById(android.R.id.content).getViewTreeObserver();
        vto.addOnGlobalLayoutListener(this);
    }

    public void StartLocationUpdates() {
        Intent intent = new Intent(GPSService.START_TRACKING);
        parentActivity.sendBroadcast(intent);
    }

    private void RequestGPSEnabled(Status status) {
        Log.i(TAG, "Got intent!! Displaying GPS settings request");
        try {
            // Show the dialog by calling startResolutionForResult(),
            // and check the result in onActivityResult().
            status.startResolutionForResult(
                    parentActivity,
                    REQUEST_CHECK_SETTINGS);
        } catch (IntentSender.SendIntentException e) {
            // Ignore the error.
        }
    }

    private void UpdateMap(final LatLng point) {
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (followUser) {
                    googleMap.animateCamera(CameraUpdateFactory.newLatLng(point));
                }
                List<LatLng> points = mapPolyline.getPoints();
                points.add(point);
                mapPolyline.setPoints(points);
            }
        });
        nativeLocationChanged(point.latitude, point.longitude);

        Intent intent = new Intent(GPSService.LOCATION_RECIEVED);
        parentActivity.sendBroadcast(intent);
    }

    private void ReceiveAllPoints(ArrayList<LatLng> points) {
        for (LatLng p : points) {
            mapPolyline.setPoints(points);
        }
    }

}