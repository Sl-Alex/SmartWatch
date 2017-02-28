package ua.com.slalex.smcenter.services;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.SmsMessage;
import android.util.Log;

import com.google.gson.Gson;

import ua.com.slalex.smcenter.BLE.BleTransferTask;

public class AlarmReceiver extends BroadcastReceiver {
    private static final String ALARM_RECEIVER_TAG = "AlarmReceiver";
    public AlarmReceiver() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        BleTransferTask task = new BleTransferTask();
        task.type = BleTransferTask.TASK_TIMESYNC;

        // Now we can start SmWatchService
        Intent mIntent = new Intent(context, SmWatchService.class);
        mIntent.putExtra(BleTransferTask.BUNDLE_TAG, new Gson().toJson(task));
        context.startService(mIntent);

        Log.d(ALARM_RECEIVER_TAG, "Alarm triggered");
    }
}
