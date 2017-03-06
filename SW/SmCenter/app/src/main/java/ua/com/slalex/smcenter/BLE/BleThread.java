package ua.com.slalex.smcenter.BLE;

import android.content.Context;
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
                    result = new BleTransferTask();
                    if (task == null)
                        break;
                    result.type = task.type;
                    switch (task.type)
                    {
                        case BleTransferTask.TASK_VERSION:
                            result.version = getFwVersion();
                            result.status = (result.version != null);
                            break;
                        case BleTransferTask.TASK_TIMESYNC:
                            result.status = setDateTime();
                            break;
                        case BleTransferTask.TASK_SMS:
                            result.status = sendNotification(task.SmsSender, task.SmsText);
                            break;
                    }
                    if (result.status) {
                        mCallback.onTransferDone(result);
                    } else {
                        break;
                    }
                }
                while ((task = mTasks.poll()) != null);
                // No new tasks at the moment, can disconnect
                mBleDevice.disconnect();
                if (result.status) {
                    break;
                }

                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Task failed, tries left: " + retries);
            }
            if (!result.status) {
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

    private String getFwVersion() {
        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_VERSION);

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Version NACK");
            return null;
        }

        String ret = rxPacket.getFwVersion();
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Version is: " + ret);
        return rxPacket.getFwVersion();
    }

    private boolean sendNotification(String header, String text) {

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_NOTIFICATION_HEADER);
        // Text size in bytes will be two times bigger because of UCS-2LE
        txPacket.setNotificationHeader(header.length(), text.length());

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Notification header NACK");
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
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Notification data NACK");
                return false;
            }

            seqNum++;
            begin += 7;
        }

        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Notification sent ACK");
        return true;
    }

    private boolean setDateTime() {
        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_DATETIME);
        txPacket.setDateTime(Calendar.getInstance());

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() != BlePacket.TYPE_ACK) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "DateTime NACK");
            return false;
        }

        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "DateTime ACK");
        return true;
    }

}
