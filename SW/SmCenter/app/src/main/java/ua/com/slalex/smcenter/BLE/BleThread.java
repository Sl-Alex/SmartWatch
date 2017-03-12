package ua.com.slalex.smcenter.BLE;

import android.content.Context;
import android.support.annotation.NonNull;
import android.util.Log;

import java.util.Calendar;
import java.util.concurrent.LinkedBlockingQueue;

import ua.com.slalex.smcenter.data.SmTextEncoder;

import ua.com.slalex.smcenter.Constants;

/**
 * This is a main BLE communication thread.
 */

public class BleThread extends Thread {

    private static final int TASK_REPEATS_MAX = 10;

    private BleDevice mBleDevice;
    private LinkedBlockingQueue<BleTransferTask> mTasks;
    private SmTextEncoder mSmTextEncoder;

    public BleThread(Context context, BleDevice device, BleThreadIface callback) {
        mBleDevice = device;
        mTasks = new LinkedBlockingQueue<>();
        mSmTextEncoder = new SmTextEncoder(context);
        mCallback = callback;
        if (!mSmTextEncoder.loadTable())
        {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Could not load font");
        }
    }

    public interface BleThreadIface {
        void onTransferDone(BleTransferTask result);
        void onThreadStarted();
        void onThreadStopped();
    }

    private BleThreadIface mCallback;

    @Override
    public void run() {
        BleTransferTask task;

        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Service is running");
        mCallback.onThreadStarted();
        while ((task = getTask()) != null)
        {
            int retries = TASK_REPEATS_MAX;
            // True by default, will be changed to false in case of a failure
            BleTransferTask result = null;
            while (retries != 0)
            {
                retries--;
                // We have at least one task
                mBleDevice.connect();
                do
                {
                    if (task == null)
                        break;
                    switch (task.type)
                    {
                        case BleTransferTask.TASK_VERSION:
                            result = getFwVersion();
                            break;
                        case BleTransferTask.TASK_TIMESYNC:
                            result = setDateTime();
                            break;
                        case BleTransferTask.TASK_SMS:
                            result = sendNotification(true, task.header, task.text);
                            break;
                        case BleTransferTask.TASK_NOTIFICATION:
                            result = sendNotification(false, task.header, task.text);
                            break;
                    }
                    if ((result != null) && result.status) {
                        mCallback.onTransferDone(result);
                    } else {
                        break;
                    }
                }
                while ((task = mTasks.poll()) != null);
                // No new tasks at the moment, can disconnect
                mBleDevice.disconnect();
                if ((result != null) && result.status) {
                    break;
                }

                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Task failed, tries left: " + retries);
            }
            if ((result != null) && (!result.status)) {
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Task failed, removed");
                mCallback.onTransferDone(result);
            }
        }
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Service is stopped");
        mCallback.onThreadStopped();
    }

    public void addTask(BleTransferTask task) {
        mTasks.add(task);
    }

    private BleTransferTask getTask() {
        BleTransferTask ret;
        try {
            ret = mTasks.take();
        } catch (InterruptedException e) {
            return null;
        }
        return ret;
    }

    @NonNull
    private BleTransferTask getFwVersion() {
        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_VERSION);

        BleTransferTask task = new BleTransferTask();
        task.type = BleTransferTask.TASK_VERSION;
        task.status = false;
        task.datetime = Calendar.getInstance();

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Version NACK");
            return task;
        }
        task.rssi = mBleDevice.getRssi();
        task.status = true;

        task.version = rxPacket.getFwVersion();
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Version is: " + task.version);
        return task;
    }

    @NonNull
    private BleTransferTask sendNotification(boolean isSms, String header, String text) {

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_NOTIFICATION_HEADER);
        // Text size in bytes will be two times bigger because of UCS-2LE
        txPacket.setNotificationHeader(header.length(), text.length());

        BleTransferTask task = new BleTransferTask();
        if (isSms)
            task.type = BleTransferTask.TASK_SMS;
        else
            task.type = BleTransferTask.TASK_NOTIFICATION;
        task.status = false;
        task.datetime = Calendar.getInstance();

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Notification header NACK");
            return task;
        }
        task.rssi = mBleDevice.getRssi();
        task.batStatus = rxPacket.getBatStatus();
        task.batLevel = rxPacket.getBatLevel();

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
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Notification data NACK");
                return task;
            }

            seqNum++;
            begin += 7;
        }
        task.status = true;

        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Notification sent ACK");
        return task;
    }

    @NonNull
    private BleTransferTask setDateTime() {
        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_DATETIME);
        txPacket.setDateTime(Calendar.getInstance());

        BleTransferTask task = new BleTransferTask();
        task.type = BleTransferTask.TASK_TIMESYNC;
        task.status = false;
        task.datetime = Calendar.getInstance();

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "DateTime NACK");
            return task;
        }
        task.rssi = mBleDevice.getRssi();
        task.status = true;
        task.batStatus = rxPacket.getBatStatus();
        task.batLevel = rxPacket.getBatLevel();

        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "DateTime ACK");
        return task;
    }

}
