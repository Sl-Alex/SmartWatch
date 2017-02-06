package ua.com.slalex.smcenter.services;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.lang.reflect.Array;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;

import ua.com.slalex.smcenter.BLE.BleDevice;
import ua.com.slalex.smcenter.BLE.BlePacket;

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

        mBleDevice.connect();

        getFwVersion();
        setDateTime(Calendar.getInstance());

        mBleDevice.disconnect();

        return super.onStartCommand(intent, flags, startId);
    }

    public synchronized String getFwVersion() {
        String ret = null;

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_VERSION);

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() == BlePacket.TYPE_ACK) {
            ret = rxPacket.getFwVersion();
            Log.d("BLE", "E: Version is: " + ret);
        }

        return ret;
    }

    public synchronized boolean setDateTime(Calendar calendar) {
        boolean ret = false;

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.setType(BlePacket.TYPE_DATETIME);
        txPacket.setDateTime(calendar);

        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() == BlePacket.TYPE_ACK) {
            Log.d("BLE", "DateTime ACK");
            ret = true;
        } else {
            Log.d("BLE", "DateTime NACK");
        }

        return ret;
    }

    @Override
    public void onDestroy() {
        mBleDevice.disconnect();
        super.onDestroy();
        Toast.makeText(this, "Ble service stopped",
                Toast.LENGTH_SHORT).show();
    }

}
