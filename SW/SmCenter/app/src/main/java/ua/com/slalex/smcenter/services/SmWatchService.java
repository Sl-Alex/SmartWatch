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

        /*
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
        */

        getFwVersion();

        return super.onStartCommand(intent, flags, startId);
    }

    public synchronized String getFwVersion() {
        String ret;

        BlePacket txPacket = new BlePacket();
        BlePacket rxPacket;
        txPacket.fillRaw(BlePacket.TYPE_VERSION);

        mBleDevice.connect();
        Log.d("BLE", "E: Sending " + txPacket.getRaw().length + " bytes, " + Arrays.toString(txPacket.getRaw()));
        rxPacket = new BlePacket(mBleDevice.transfer(txPacket.getRaw()));
        if (rxPacket.getType() == BlePacket.TYPE_ACK) {
            Log.d("BLE", "E: Version is: " + rxPacket.getFwVersion());
        }
        Log.d("BLE","E: Disconnecting...");
        mBleDevice.disconnect();

        ret = "TBD";
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
