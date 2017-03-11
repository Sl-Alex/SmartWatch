package ua.com.slalex.smcenter.services;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.drawable.Icon;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

import com.google.gson.Gson;

import java.util.ArrayList;

import ua.com.slalex.smcenter.BLE.BleDevice;
import ua.com.slalex.smcenter.BLE.BleThread;
import ua.com.slalex.smcenter.BLE.BleTransferTask;
import ua.com.slalex.smcenter.MainActivity;
import ua.com.slalex.smcenter.R;

import ua.com.slalex.smcenter.Constants;

public class SmWatchService extends Service implements BleThread.BleThreadIface {

    private static final int NOTIFICATION_ID = 1;
    private static final int RETURN_VALUE = START_STICKY;
    private static final boolean USE_ACTIONS = false;
    private static final int LOG_SIZE = 20;

    BleThread mBleThread = null;
    BleDevice mBleDevice = null;
    private SmWatchServiceIface mCallbackIface = null;

    SmWatchBinder mBinder = new SmWatchBinder();
    public class SmWatchBinder extends Binder {
        public SmWatchService getService() {
            return SmWatchService.this;
        }
    }

    public interface SmWatchServiceIface {
        void onLogUpdated(ArrayList<BleTransferTask> results);
    }

    ArrayList<BleTransferTask> mLastResults = null;

    public SmWatchService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        mCallbackIface = null;
        return super.onUnbind(intent);
    }

    @Override
    public void onRebind(Intent intent) {
        super.onRebind(intent);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mBleDevice = new BleDevice();
        mLastResults = new ArrayList<>();

        showForegroundNotification("Service is created");

        // HMSoft module
        //mBleDevice.setBtAddress("74:DA:EA:B2:14:CF", this);
        // SmartWatch prototype
        mBleDevice.setBtAddress("74:DA:EA:B2:A2:2B", this);

        Intent myIntent = new Intent(this, AlarmReceiver.class);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, myIntent,0);

        AlarmManager alarmManager = (AlarmManager)getSystemService(ALARM_SERVICE);
        alarmManager.setInexactRepeating(AlarmManager.RTC_WAKEUP, System.currentTimeMillis(), AlarmManager.INTERVAL_HOUR, pendingIntent);
    }

    public void setCallbackIface(SmWatchServiceIface iface) {
        mCallbackIface = iface;
    }

    @Override
    public synchronized int onStartCommand(Intent intent, int flags, int startId) {
        if ((mBleThread == null) || (!mBleThread.isAlive()))
        {
            startThread();
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Service (re)started");
        }
        if ((mBleThread != null) && (!mBleThread.isAlive()))
        {
            mBleThread.interrupt();
            mBleDevice.disconnect();
            startThread();
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Service (re)started abnormally");
        }

        if (intent == null)
            return RETURN_VALUE;

        Bundle bundle = intent.getExtras();
        if (bundle == null)
            return RETURN_VALUE;

        String taskJson = bundle.getString(BleTransferTask.BUNDLE_TAG, null);
        if (taskJson == null)
            return RETURN_VALUE;

        BleTransferTask task = new Gson().fromJson(taskJson, BleTransferTask.class);
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "New task of type " + task.type);
        mBleThread.addTask(task);
        return RETURN_VALUE;
    }

    private void startThread() {
        mBleThread = new BleThread(getApplicationContext(), mBleDevice, this);
        mBleThread.start();
    }

    @Override
    public void onDestroy() {
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "onDestroy");
        mBleDevice.disconnect();
        // Null task will stop mTasksThread
        mBleThread.addTask(null);
        try {
            mBleThread.join(2000);
        } catch (InterruptedException e) {
            mBleThread.interrupt();
        }
        mBleDevice.disconnect();
        super.onDestroy();
    }

    private void showForegroundNotification(String contentText) {
        // Create intent that will bring our app to the front, as if it was tapped in the app
        // launcher
        Context context = getApplicationContext();
        Intent showTaskIntent = new Intent(context, MainActivity.class);
        showTaskIntent.setAction(Intent.ACTION_MAIN);
        showTaskIntent.addCategory(Intent.CATEGORY_LAUNCHER);
        showTaskIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        PendingIntent contentIntent = PendingIntent.getActivity(
                context,
                0,
                showTaskIntent,
                PendingIntent.FLAG_UPDATE_CURRENT);

        Notification.Builder builder = new Notification.Builder(context)
                .setContentTitle(getString(R.string.app_name))
                .setContentText(contentText)
                .setSmallIcon(R.drawable.ic_watch_black)
                .setWhen(System.currentTimeMillis())
                .setContentIntent(contentIntent);
        if (USE_ACTIONS) {
            Notification.Action addNewAction;
            Notification.Action infoAction;
            Notification.Action homeAction;

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                addNewAction = new Notification.Action.Builder(
                        Icon.createWithResource(context,R.drawable.ic_add_white),"Home", contentIntent)
                        .build();
                infoAction = new Notification.Action.Builder(
                        Icon.createWithResource(context,R.drawable.ic_info_black),"Add", contentIntent)
                        .build();
                homeAction = new Notification.Action.Builder(
                        Icon.createWithResource(context, R.drawable.ic_home_black),"Home", contentIntent)
                        .build();
            } else {
                //noinspection deprecation
                addNewAction = new Notification.Action(R.drawable.ic_add_white,"Home", contentIntent);
                //noinspection deprecation
                infoAction = new Notification.Action(R.drawable.ic_info_black,"Add", contentIntent);
                //noinspection deprecation
                homeAction = new Notification.Action(R.drawable.ic_home_black,"Home", contentIntent);
            }
            builder.addAction(addNewAction)
                    .addAction(homeAction)
                    .addAction(infoAction);
        }
        startForeground(NOTIFICATION_ID, builder.build());
    }

    public synchronized ArrayList<BleTransferTask> getLastResults() {
        return mLastResults;
    }

    @Override
    public synchronized void onTransferDone(BleTransferTask result) {
        if (mLastResults.size() > LOG_SIZE)
        {
            mLastResults.remove(0);
        }
        mLastResults.add(result);
        if (result.status)
            showForegroundNotification(BleTransferTask.toString(result.type) + " succeeded, RSSI = " + result.rssi);
        else
            showForegroundNotification(BleTransferTask.toString(result.type) + " failed" );
        if (mCallbackIface != null)
            mCallbackIface.onLogUpdated(mLastResults);
    }

    @Override
    public void onThreadStarted() {
        showForegroundNotification("BLE service started" );
    }

    @Override
    public void onThreadStopped() {
        showForegroundNotification("BLE service stopped" );
    }
}
