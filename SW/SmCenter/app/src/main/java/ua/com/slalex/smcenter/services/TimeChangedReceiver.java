package ua.com.slalex.smcenter.services;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.google.gson.Gson;

import ua.com.slalex.smcenter.BLE.BleTransferTask;
import ua.com.slalex.smcenter.Constants;

public class TimeChangedReceiver extends BroadcastReceiver {

    public TimeChangedReceiver() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        BleTransferTask task = new BleTransferTask();
        task.type = BleTransferTask.TASK_TIMESYNC;

        // Now we can start SmWatchService
        Intent mIntent = new Intent(context, SmWatchService.class);
        mIntent.putExtra(BleTransferTask.BUNDLE_TAG, new Gson().toJson(task));
        context.startService(mIntent);

        // Log it
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Time has been changed");
    }
}
