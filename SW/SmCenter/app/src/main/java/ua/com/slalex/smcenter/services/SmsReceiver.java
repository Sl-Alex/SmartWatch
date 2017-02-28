package ua.com.slalex.smcenter.services;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.telephony.SmsMessage;
import android.util.Log;

import com.google.gson.Gson;

import ua.com.slalex.smcenter.BLE.BleTransferTask;

public class SmsReceiver extends BroadcastReceiver {
    private static final String ACTION = "android.provider.Telephony.SMS_RECEIVED";
    private static final String SMS_RECEIVER_TAG = "SmsReceiver";
    public SmsReceiver() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        // Check if input is correct
        if (intent == null || intent.getAction() == null &&
                ACTION.compareToIgnoreCase(intent.getAction()) != 0)
            return;

        // Get incoming messages
        Bundle bundle = intent.getExtras();
        //noinspection SpellCheckingInspection pdus is a correct spelling
        Object[] pduArray = (Object[]) bundle.get("pdus");
        if (pduArray == null)
            return;
        SmsMessage[] messages = new SmsMessage[pduArray.length];
        String format = bundle.getString("format");
        for (int i = 0; i < pduArray.length; i++) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                messages[i] = SmsMessage.createFromPdu((byte[]) pduArray[i], format);
            } else {
                //noinspection deprecation
                messages[i] = SmsMessage.createFromPdu((byte[]) pduArray[i]);
            }
        }

        BleTransferTask task = new BleTransferTask();
        task.type = BleTransferTask.TASK_SMS;
        task.SmsSender = messages[0].getDisplayOriginatingAddress();

        StringBuilder bodyText = new StringBuilder();
        for (SmsMessage message : messages) {
            bodyText.append(message.getMessageBody());
        }
        task.SmsText = bodyText.toString();

        // All SMS information is extracted, we can pass this information to SmWatchService
        Intent mIntent = new Intent(context, SmWatchService.class);
        mIntent.putExtra(BleTransferTask.BUNDLE_TAG, new Gson().toJson(task));
        context.startService(mIntent);

        // Log it
        Log.d(SMS_RECEIVER_TAG, "New SMS received");
    }
}
