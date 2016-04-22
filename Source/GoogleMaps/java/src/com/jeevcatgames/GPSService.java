package com.jeevcatgames;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import com.jeevcatgames.IGPSService;

import com.epicgames.ue4.GameActivity;

/**
 * Custom Service which will continually pull GPS positions until stopped.
 * Copyright 2016, Sam Jeeves. All rights reserved.
 */


public class GPSService extends Service
{
    private NotificationManager mNM;

    private static final String TAG = "GPSService";
    private static final int LOCATION_INTERVAL = 1000;
    private static final float LOCATION_DISTANCE = 10f;

    // Unique Identification Number for the Notification.
    // We use it on Notification start, and to cancel it.
    private int NOTIFICATION = 1234;
    // This is the object that receives interactions from clients.  See
    // RemoteService for a more complete example.

    private final IGPSService.Stub binder = new IGPSService.Stub() {
        public int getPid(){
            return 2345;
        }
        public void basicTypes(int anInt, long aLong, boolean aBoolean,
                               float aFloat, double aDouble, String aString) {
            // Does nothing
        }
    };
    @Override
    public void onCreate() {
        super.onCreate();
        mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);

        // Display a notification about us starting.  We put an icon in the status bar.
        showNotification();
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
    }

    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }
    /**
     * Show a notification while this service is running.
     */
    private void showNotification() {
        // In this sample, we'll use the same text for the ticker and the expanded notification
        CharSequence text = "Whats up";

        // The PendingIntent to launch our activity if the user selects this notification
        Intent intent =  new Intent(getApplicationContext(), GameActivity.class);
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
}