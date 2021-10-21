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


public class SpeedLimitNotifierService extends QtService
{
    private static final String TAG = "SpeedLimitService";

    public static final String CHANNEL_ID = "SpeedLimitServiceFGChannel";

    @Override
    public void onCreate() {
        super.onCreate();
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

       // if( intent!=null ) {
        //    String action = intent.getAction();
         //   switch (action) {
          //      case "ACTION_START_FOREGROUND_SERVICE":

                    startForegroundService();


            /*    break;
            }

        }*/

        int ret = super.onStartCommand(intent, flags, startId);
        return START_NOT_STICKY;
    } 


    private void startForegroundService() {
        createNotificationChannel();
        //Intent intent = new Intent();
        //PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, 0);

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(this, CHANNEL_ID);

        /*NotificationCompat.BigTextStyle bigTextStyle = new NotificationCompat.BigTextStyle();
        bigTextStyle.setBigContentTitle("Music player implemented by foreground service.");
        bigTextStyle.bigText("Android foreground service is a android service which can run in foreground always, it can be controlled by user via notification.");
        builder.setStyle(bigTextStyle);
        builder.setWhen(System.currentTimeMillis());

        builder.setSmallIcon(R.mipmap.ic_launcher);
        //Bitmap largeIconBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.icon_music_32);
        //builder.setLargeIcon(largeIconBitmap);
        builder.setPriority(Notification.PRIORITY_MAX);
        builder.setFullScreenIntent(pendingIntent, true);*/


        Notification notification = notificationBuilder.setOngoing(true)
                    //.setSmallIcon(R.drawable.icon_1)
                    .setSmallIcon(android.R.drawable.ic_notification_overlay)
                    .setContentTitle("App is running in background")
                    .setPriority(NotificationManager.IMPORTANCE_MIN)
                    .setCategory(Notification.CATEGORY_SERVICE)
                    .build();

        startForeground(1142, notification);
    }

    private void createNotificationChannel() {
       // if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(CHANNEL_ID, "Foreground Service Channel", NotificationManager.IMPORTANCE_DEFAULT);
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
        //}
    }
}
