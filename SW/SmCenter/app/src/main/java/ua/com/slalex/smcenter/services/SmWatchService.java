package ua.com.slalex.smcenter.services;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

import com.google.gson.Gson;

import java.util.Calendar;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

import ua.com.slalex.smcenter.BLE.BleDevice;
import ua.com.slalex.smcenter.BLE.BlePacket;
import ua.com.slalex.smcenter.BLE.BleTransferTask;
import ua.com.slalex.smcenter.data.SmTextEncoder;

public class SmWatchService extends Service {

    public static final String SERVICE_TAG = "SmWatchService";

    BleDevice mBleDevice;
    LinkedBlockingQueue<BleTransferTask> mTasks;
    Thread mTasksThread;

    SmTextEncoder mSmTextEncoder;

//    boolean isStarted;
    AtomicBoolean isRunning;

    public SmWatchService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mBleDevice = new BleDevice();

        mSmTextEncoder = new SmTextEncoder(this);
        if (!mSmTextEncoder.loadTable())
        {
            Log.d(SERVICE_TAG, "Could not load font");
        }

        isRunning = new AtomicBoolean();
        isRunning.set(false);

        // HMSoft module
        //mBleDevice.setBtAddress("74:DA:EA:B2:14:CF", this);
        // SmartWatch prototype
        mBleDevice.setBtAddress("74:DA:EA:B2:A2:2B", this);
        mTasks = new LinkedBlockingQueue<>();

        Intent myIntent = new Intent(this, AlarmReceiver.class);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, myIntent,0);

        AlarmManager alarmManager = (AlarmManager)getSystemService(ALARM_SERVICE);
        alarmManager.setInexactRepeating(AlarmManager.RTC_WAKEUP, System.currentTimeMillis(), AlarmManager.INTERVAL_HOUR, pendingIntent);
    }

    @Override
    public synchronized int onStartCommand(Intent intent, int flags, int startId) {
        Bundle bundle = null;
        String taskJson = null;

        if (intent != null)
            bundle = intent.getExtras();

        if (bundle != null)
            taskJson = bundle.getString(BleTransferTask.BUNDLE_TAG, null);

        if (taskJson != null) {
            BleTransferTask task = new Gson().fromJson(taskJson, BleTransferTask.class);

            Log.d(SERVICE_TAG, "New task of type " + task.type);

            addTask(task);
        }

        if ((mTasksThread != null) && (!mTasksThread.isAlive()))
        {
            mTasksThread.interrupt();
            isRunning.set(false);
        }

        //BleTransferTask temp_task = new BleTransferTask();
        //temp_task.type = BleTransferTask.TASK_SMS;
        //temp_task.SmsSender = "Schätzchen";
        //temp_task.SmsText = "Ich küsse dich ;)";
        //addTask(temp_task);


        if (!isRunning.compareAndSet(false,true)) {
            return START_STICKY;
        }

        Log.d(SERVICE_TAG,"Service (re)started");

        mTasksThread = new Thread(new Runnable() {
            @Override
            public void run() {
                BleTransferTask task;

                while ((task = getTask()) != null)
                {
                    // We have at least one task
                    mBleDevice.connect();
                    do
                    {
                        switch (task.type)
                        {
                            case BleTransferTask.TASK_VERSION:
                                getFwVersion();
                                break;
                            case BleTransferTask.TASK_TIMESYNC:
                                setDateTime();
                                break;
                            case BleTransferTask.TASK_SMS:
                                sendNotification(task.SmsSender, task.SmsText);
                                break;
                        }
                    }
                    while ((task = mTasks.poll()) != null);

                    // No new tasks at the moment, can disconnect
                    mBleDevice.disconnect();
                }
                isRunning.set(false);
            }
        });
        mTasksThread.start();


        BleTransferTask task = new BleTransferTask();
        task.type = BleTransferTask.TASK_VERSION;
        addTask(task);
        task = new BleTransferTask();
        task.type = BleTransferTask.TASK_TIMESYNC;
        addTask(task);

        return START_STICKY;
    }

    private String getFwVersion() {
        String ret = null;

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_VERSION);

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() == BlePacket.TYPE_ACK) {
            ret = rxPacket.getFwVersion();
            Log.d(SERVICE_TAG, "Version is: " + ret);
        }

        return ret;
    }

    private boolean sendNotification(String header, String text) {
        boolean ret = false;

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_NOTIFICATION_HEADER);
        // Text size in bytes will be two times bigger because of UCS-2LE
        txPacket.setNotificationHeader(header.length(), text.length());

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(SERVICE_TAG, "Notification header NACK");
            return false;
        }

        String sum_str = header + text;
        int total_length = sum_str.length();

        int begin = 0;
        int end; // 7 symbols in a single packet
        int seqNum = 0;
        while(begin < total_length)
        {
            end = begin + 7;
            if (end > sum_str.length())
                end = sum_str.length();
            String temp = sum_str.substring(begin,end);

            txPacket.setType(BlePacket.TYPE_NOTIFICATION_DATA);
            txPacket.setNotificationData(seqNum, temp, mSmTextEncoder);
            rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
            if (rxPacket.getType() != BlePacket.TYPE_ACK) {
                Log.d(SERVICE_TAG, "Notification data NACK");
                return false;
            }

            seqNum++;
            begin += 7;
        }

        return ret;
    }

    private boolean setDateTime() {
        boolean ret = false;

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_DATETIME);
        txPacket.setDateTime(Calendar.getInstance());

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() == BlePacket.TYPE_ACK) {
            Log.d(SERVICE_TAG, "DateTime ACK");
            ret = true;
        }

        return ret;
    }

    public void addTask(BleTransferTask task)
    {
        mTasks.add(task);
        Log.d(SERVICE_TAG, "Task added");
    }

    public BleTransferTask getTask()
    {
        BleTransferTask ret;
        try {
            ret = mTasks.take();
        } catch (InterruptedException e) {
            return null;
        }
        return ret;
    }

    @Override
    public void onDestroy() {
        Log.d(SERVICE_TAG, "onDestroy");
        mBleDevice.disconnect();
        // Null task will stop mTasksThread
        mTasks.add(null);
        try {
            mTasksThread.join(2000);
        } catch (InterruptedException e) {
            mTasksThread.interrupt();
        }
        super.onDestroy();
    }

}
