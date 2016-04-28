package com.jeevcatgames;

/**
 * Custom Dialog which holds a MapFragment for use in UnrealEngine
 * Copyright 2016, Sam Jeeves. All rights reserved.
 *
 *
 */

import android.app.Activity;
import android.app.Dialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.IntentSender;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
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
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;
import java.util.List;


public class UEMapDialog extends Dialog implements ViewTreeObserver.OnGlobalLayoutListener, OnMapReadyCallback {

    @SuppressWarnings("JniMissingFunction")
    public native void nativeLocationChanged(double lat, double lng, long time);
    @SuppressWarnings("JniMissingFunction")
    public native void nativeAllPoints(double[] latArray, double[] lngArray, long[] timeArray);
    @SuppressWarnings("JniMissingFunction")
    public native void nativeMapReady();

    public static final String USER_REQUEST_GPS_SETTING = "com.jeevcatgames.UEMapDialog.USER_REQUEST_GPS_SETTING";
    public static final String UPDATE_MAP = "com.jeevcatgames.UEMapDialog.UPDATE_MAP";
    public static final String RECEIVE_ALL_POINTS = "com.jeevcatgames.UEMapDialog.RECEIVE_ALL_POINTS";
    private static final String TAG = "UEMapDialog";
    private static final int REQUEST_CHECK_SETTINGS = 0x1;
    private int posX, posY, sizeX, sizeY;
    private Activity parentActivity;
    private View frame;
    private int layoutId, mapContainerId;
    private boolean followUser, trackingEnabled, reconnectToTrackingService;
    private MapFragment mapFragment;
    private GoogleMap googleMap;
    private Polyline mapPolyline;
    private BroadcastReceiver receiver;


    // Constructor
    public UEMapDialog(final Context context, boolean trackingEnabled,
                       boolean reconnectToTrackingService, int pX, int pY, int sX, int sY) {
        super(context, android.R.style.Theme_Panel);
        parentActivity = (Activity) context;
        followUser = true;

        this.posX = pX;
        this.posY = pY;
        this.sizeX = sX;
        this.sizeY = sY;
        this.trackingEnabled = trackingEnabled;
        this.reconnectToTrackingService = reconnectToTrackingService;
    }

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
        intentFilter.addAction(UPDATE_MAP);
        intentFilter.addAction(RECEIVE_ALL_POINTS);


        receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.i(TAG, "BroadcastReceiver received: " + intent.getAction());
                if (intent.getAction().equals(USER_REQUEST_GPS_SETTING))
                    RequestGPSEnabled((Status) intent.getParcelableExtra("Status"));
                if (intent.getAction().equals(UPDATE_MAP))
                    UpdateMap((GPSService.LatLngTime) intent.getSerializableExtra("LatLngTime"),
                            intent.hasExtra("justPanCamera"));
                if (intent.getAction().equals(RECEIVE_ALL_POINTS)) {
                    ArrayList<GPSService.LatLngTime> points =
                            (ArrayList<GPSService.LatLngTime>) intent.getSerializableExtra("Points");
                    if (points != null) {
                        ReceiveAllPoints(points);
                    } else {
                        float lat[] = intent.getFloatArrayExtra("lat");
                        float lng[] = intent.getFloatArrayExtra("lng");
                        ReceiveAllPoints(lat,lng);
                    }
                }
            }
        };

        parentActivity.registerReceiver(receiver, intentFilter);

        if(trackingEnabled) {
            if (reconnectToTrackingService) {
                parentActivity.sendBroadcast(new Intent(GPSService.REQUEST_ALL_POINTS));
                Log.i(TAG, "Service already running. Requesting all points.");
            } else {
                parentActivity.sendBroadcast(new Intent(GPSService.CONNECT_TO_GOOGLE_API));
            }
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
        if(trackingEnabled) {
            map.setMyLocationEnabled(true);
            map.animateCamera(CameraUpdateFactory.zoomTo(18));
        }
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
        if(!trackingEnabled)
            nativeMapReady();
    }


    @Override
    public void onGlobalLayout() {
        Log.i(TAG, "In OnResume OnGlobalLayoutListener");
        ViewTreeObserver vto = parentActivity.findViewById(android.R.id.content).getViewTreeObserver();
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN) {
            vto.removeGlobalOnLayoutListener(this);
        } else {
            vto.removeOnGlobalLayoutListener(this);
        }
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

    private void RequestGPSEnabled(Status status) {
        Log.i(TAG, "Displaying GPS settings request");
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

    private void UpdateMap(final GPSService.LatLngTime point, final boolean justPanCamera) {
        parentActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (followUser) {
                    googleMap.animateCamera(CameraUpdateFactory.newLatLng(point.toLatLng()));
                }
                if(!justPanCamera) {
                    List<LatLng> points = mapPolyline.getPoints();
                    points.add(point.toLatLng());
                    mapPolyline.setPoints(points);
                }
            }
        });

        Intent intent = new Intent(GPSService.LOCATION_RECIEVED);
        parentActivity.sendBroadcast(intent);

        nativeLocationChanged(point.Latitude, point.Longitude, point.UTCTime);
    }

    private void ReceiveAllPoints(ArrayList<GPSService.LatLngTime> points) {
        Log.i(TAG, "Receiving all points.");
        ArrayList<LatLng> polyPoints = new ArrayList<LatLng>();
        int size = points.size();
        double[] latArray = new double[size];
        double[] lngArray = new double[size];
        long[] timeArray = new long[size];
        for(int i=0; i<size; i++){
            latArray[i] = points.get(i).Latitude;
            lngArray[i] = points.get(i).Longitude;
            timeArray[i] = points.get(i).UTCTime;
            polyPoints.add(points.get(i).toLatLng());
        }
        mapPolyline.setPoints(polyPoints);
        nativeAllPoints(latArray, lngArray, timeArray);
    }

    private void ReceiveAllPoints(float[] lat, float[] lng) {
        LatLngBounds.Builder b = new LatLngBounds.Builder();
        ArrayList<LatLng> polyPoints = new ArrayList<LatLng>();
        for(int i = 0; i < lat.length; i++) {
            LatLng ll = new LatLng(lat[i],lng[i]);
            polyPoints.add(ll);
            b.include(ll);
        }
        mapPolyline.setPoints(polyPoints);
        LatLngBounds bounds = b.build();
        googleMap.animateCamera(CameraUpdateFactory.newLatLngBounds(bounds, 5));
    }

}