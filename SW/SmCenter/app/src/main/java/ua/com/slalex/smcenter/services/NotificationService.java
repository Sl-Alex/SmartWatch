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

        //Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Package = " + pack);
        //Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Ticker = " + ticker);
        //Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Title = " + title);
        //Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Text = " + text);
    }
}
