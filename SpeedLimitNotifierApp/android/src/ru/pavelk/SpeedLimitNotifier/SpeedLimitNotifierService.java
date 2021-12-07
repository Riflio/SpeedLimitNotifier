package ru.pavelk.SpeedLimitNotifier;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtService;


import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.os.Build;
import android.os.IBinder;
import android.support.annotation.Nullable;
import android.support.v4.app.NotificationCompat;

import android.os.SystemClock;
import android.media.AudioManager;
import android.media.ToneGenerator;


public class SpeedLimitNotifierService extends QtService
{
    private static final String TAG = "SpeedLimitService";
    public static final String CHANNEL_ID = "SpeedLimitServiceFGChannel";
    public static final String ACTION_START_FOREGROUND_SERVICE = "ACTION_START_FOREGROUND_SERVICE";

    private ToneGenerator _toneGenerator;

    @Override
    public void onCreate() {
        super.onCreate();
        _toneGenerator = new ToneGenerator(4, 100);
        Log.i(TAG, "Creating Service");
    }
    
    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "Destroying Service");
    }
    
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        Log.i(TAG, "onStartCommand");

        if( intent!=null ) {
            String action = intent.getAction();
            switch (action) {
                case "ACTION_START_FOREGROUND_SERVICE":
                    startForegroundService();
                break;
            }
        }

        int ret = super.onStartCommand(intent, flags, startId);
        return START_NOT_STICKY;
    } 

    public static void toStartService(Context context)
    {
        Intent serviceIntent = new Intent(context, SpeedLimitNotifierService.class);
        serviceIntent.setAction(SpeedLimitNotifierService.ACTION_START_FOREGROUND_SERVICE);
        context.startService(serviceIntent);
    }

    public static void toStopService(Context context)
    {
        context.stopService(new Intent(context, SpeedLimitNotifierService.class));
    }

    private void startForegroundService()
    {
        Intent resultIntent = new Intent(this, ru.pavelk.SpeedLimitNotifier.MainActivity.class);
        resultIntent.setAction("android.intent.action.MAIN");
        resultIntent.addCategory("android.intent.category.LAUNCHER");

        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, resultIntent, 0);

        // if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
             NotificationChannel serviceChannel = new NotificationChannel(CHANNEL_ID, "Speed limiter main chanel", NotificationManager.IMPORTANCE_DEFAULT);
             NotificationManager manager = getSystemService(NotificationManager.class);
             manager.createNotificationChannel(serviceChannel);
         //}

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, CHANNEL_ID);

        Notification notification = notificationBuilder.setOngoing(true)
                    .setSmallIcon(R.drawable.notify)
                    .setContentTitle("Running")
                    .setPriority(NotificationManager.IMPORTANCE_MIN)
                    .setCategory(Notification.CATEGORY_SERVICE)
                    .setContentIntent(pendingIntent)
                    .build();

        startForeground(1142, notification);
    }

    public void notifySignal()
    {
        _toneGenerator.startTone(ToneGenerator.TONE_DTMF_7);
        SystemClock.sleep(200);
        _toneGenerator.stopTone();
    }

}
