package ua.com.slalex.smcenter.services;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import ua.com.slalex.smcenter.BLE.BleDevice;

public class SmWatchService extends Service {

    BleDevice mBleDevice;

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
        // SmartWatch prototype
        //mBleDevice.getInstance().init("74:DA:EA:B2:A2:2B", this);
        // HMSoft module
        mBleDevice = new BleDevice();
        mBleDevice.init("74:DA:EA:B2:14:CF", this);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d("BLE","Service started");
        Toast.makeText(this, "Ble service started",
                Toast.LENGTH_SHORT).show();

        Log.d("BLE","E: Connecting...");
        mBleDevice.connect();
        String data = "0123456789ABCDEFGHIJ";
        byte[] data_out = data.getBytes(StandardCharsets.US_ASCII);
        Log.d("BLE", "E: Sending " + data_out.length + " bytes");
        byte[] data_in = null;
        data_in = mBleDevice.transfer(data_out);
        Log.d("BLE", "E: Received: " + Arrays.toString(data_in));
        Log.d("BLE","E: Disconnecting...");
        mBleDevice.disconnect();
        Log.d("BLE","E: Disconnected");

        return super.onStartCommand(intent, flags, startId);
    }

    public synchronized void sendNotification() {

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Toast.makeText(this, "Ble service stopped",
                Toast.LENGTH_SHORT).show();
    }

}
