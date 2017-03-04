package ua.com.slalex.smcenter;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by Sl-Alex on 14.12.2016.
 */

public class AppSettings {
    private static final String APP_PREFERENCES = "AppSettings";
    private static final String SETTINGS_SEND_NOTIFICATIONS = "sendNotifications";
    private static final String SETTINGS_SEND_DATETIME = "sendDateTime";
    private static final String SETTINGS_FILTER_TYPE = "filterType";

    public static final int FT_NONE = 0;
    public static final int FT_BLACKLIST = 1;
    public static final int FT_WHITELIST = 2;

    private SharedPreferences mPreferences;

    public AppSettings(Context ctx) {
        mPreferences = ctx.getSharedPreferences(APP_PREFERENCES, Context.MODE_PRIVATE);
    }

    public void setSendNotifications(boolean value)
    {
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putBoolean(SETTINGS_SEND_NOTIFICATIONS, value);
        editor.apply();
    }

    public boolean getSendNotifications()
    {
        return mPreferences.getBoolean(SETTINGS_SEND_NOTIFICATIONS, true);
    }

    public void setSendDateTime(boolean value)
    {
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putBoolean(SETTINGS_SEND_DATETIME, value);
        editor.apply();
    }

    public boolean getSendDateTime()
    {
        return mPreferences.getBoolean(SETTINGS_SEND_DATETIME, true);
    }

    public void setFilterType(int value)
    {
        SharedPreferences.Editor editor = mPreferences.edit();
        editor.putInt(SETTINGS_FILTER_TYPE, value);
        editor.apply();
    }

    public int getFilterType()
    {
        return mPreferences.getInt(SETTINGS_FILTER_TYPE, FT_NONE);
    }
}
