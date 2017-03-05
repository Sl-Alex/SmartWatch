package ua.com.slalex.smcenter.services;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.provider.Settings;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.PermissionChecker;
import android.telephony.SmsMessage;
import android.util.Log;

import com.google.gson.Gson;

import ua.com.slalex.smcenter.BLE.BleTransferTask;

import ua.com.slalex.smcenter.Constants;

public class SmsReceiver extends BroadcastReceiver {
    private static final String ACTION = "android.provider.Telephony.SMS_RECEIVED";

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
        task.SmsSender = messages[0].getOriginatingAddress();

        StringBuilder bodyText = new StringBuilder();
        for (SmsMessage message : messages) {
            bodyText.append(message.getMessageBody());
        }
        task.SmsText = bodyText.toString();

        if (Settings.Secure.getString(context.getContentResolver(),
                "enabled_notification_listeners").contains(context.getPackageName()))
        {
            //Resolving the contact name from the contacts.
            Uri lookupUri = Uri.withAppendedPath(ContactsContract.PhoneLookup.CONTENT_FILTER_URI, Uri.encode(task.SmsSender));
            Cursor c = context.getContentResolver().query(lookupUri, new String[]{ContactsContract.Data.DISPLAY_NAME},null,null,null);
            try {
                c.moveToFirst();
                task.SmsSender = c.getString(0);
            } catch (Exception e) {
                // TODO: handle exception
            }finally{
                c.close();
            }
        }

        // All SMS information is extracted, we can pass this information to SmWatchService
        Intent mIntent = new Intent(context, SmWatchService.class);
        mIntent.putExtra(BleTransferTask.BUNDLE_TAG, new Gson().toJson(task));
        context.startService(mIntent);

        // Log it
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "New SMS received");
    }
}
