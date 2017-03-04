package ua.com.slalex.smcenter.services;

import android.content.Context;
import android.os.Bundle;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

import ua.com.slalex.smcenter.Constants;

/**
 * Created by Sl-Alex on 25.12.2016.
 */

public class NotificationService extends NotificationListenerService {

    Context context;

    @Override
    public void onCreate() {
        super.onCreate();
        context = getApplicationContext();
    }

    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {
        super.onNotificationPosted(sbn);
        String pack = sbn.getPackageName();
        String ticker ="";
        if(sbn.getNotification().tickerText !=null) {
            ticker = sbn.getNotification().tickerText.toString();
        }
        Bundle extras = sbn.getNotification().extras;
        String title = extras.getString("android.title");
        String text = extras.getCharSequence("android.text").toString();
        //int id1 = extras.getInt(Notification.EXTRA_SMALL_ICON);
        //Bitmap id = sbn.getNotification().largeIcon;


        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Package " + pack);
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Ticker " + ticker);
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Title " + title);
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Text " + text);

/*        Intent msgrcv = new Intent("Msg");
        msgrcv.putExtra("package", pack);
        msgrcv.putExtra("ticker", ticker);
        msgrcv.putExtra("title", title);
        msgrcv.putExtra("text", text);
        if(id != null) {
            ByteArrayOutputStream stream = new ByteArrayOutputStream();
            id.compress(Bitmap.CompressFormat.PNG, 100, stream);
            byte[] byteArray = stream.toByteArray();
            msgrcv.putExtra("icon",byteArray);
        }
        LocalBroadcastManager.getInstance(context).sendBroadcast(msgrcv);
        */
    }
}
