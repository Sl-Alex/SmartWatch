package ua.com.slalex.smcenter.services;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.ScanCallback;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

import java.util.List;

public class SmWatchService extends Service {

    private static final String WRITE_CHARACTERISTIC_ID = "0000FFE1-0000-1000-8000-00805F9B34FB";
    private static final String READ_CHARACTERISTIC_ID = "0000FFE1-0000-1000-8000-00805F9B34FB";

    BluetoothManager mBtManager;
    BluetoothAdapter mBtAdapter;
    ScanCallback mBleScanCallback;
    BluetoothDevice mBleDevice;
    BluetoothGatt mBleGatt;
    BluetoothGattCallback mBleGattCallback;
    BluetoothGattCharacteristic mWriteCharacteristic, mNotifyCharacteristic;
    boolean mBleActive;
    int mConnectionState;

    Context mContext;

    public SmWatchService() {
        mBleActive = false;
        mWriteCharacteristic = null;
        mNotifyCharacteristic = null;
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mContext = this;
        Log.d("BLE","Service created");
        Toast.makeText(this, "Ble service created",
                Toast.LENGTH_SHORT).show();

        // Ble GATT callback
        mBleGattCallback = new BluetoothGattCallback() {
            @Override
            public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
                super.onCharacteristicChanged(gatt, characteristic);
                Log.d("BLE","Characteristic changed");
            }

            @Override
            public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                super.onConnectionStateChange(gatt, status, newState);
                Log.d("BLE","Characteristic changed");
                mConnectionState = newState;
                if (mConnectionState == BluetoothProfile.STATE_CONNECTED)
                {
                    mBleActive = true;
                    Log.d("BLE","Discovering services");
                    mBleGatt.discoverServices();
                }
                else
                {
                    mBleActive = false;
                }
                //BluetoothProfile.STATE_CONNECTED;
            }

            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                super.onServicesDiscovered(gatt, status);
                if (status != BluetoothGatt.GATT_SUCCESS)
                    return;
                Log.d("BLE","Services discovered");
                List<BluetoothGattService> services = gatt.getServices();
                for (BluetoothGattService service : services) {
                    List<BluetoothGattCharacteristic> characteristics = service.getCharacteristics();
                    for (BluetoothGattCharacteristic characteristic : characteristics) {
                        if (characteristic.getUuid().toString().equalsIgnoreCase(WRITE_CHARACTERISTIC_ID))
                        {
                            mWriteCharacteristic = characteristic;
                            mWriteCharacteristic.setValue("Test string :)");
                            mBleGatt.writeCharacteristic(mWriteCharacteristic);
                        }
                        if (characteristic.getUuid().toString().equalsIgnoreCase(READ_CHARACTERISTIC_ID)) {
                            mNotifyCharacteristic = characteristic;
                            mBleGatt.setCharacteristicNotification(characteristic, true);
                        }
                    }
                }
            }
        };

        mBtManager = (BluetoothManager)mContext.getSystemService(Context.BLUETOOTH_SERVICE);
        mBtAdapter = mBtManager.getAdapter();
        // "74:DA:EA:B2:A2:2B"
        // mBleDevice = mBtAdapter.getRemoteDevice("74:DA:EA:B2:A2:2B"); ///< SmartWatch
        mBleDevice = mBtAdapter.getRemoteDevice("74:DA:EA:B2:14:CF"); ///< HMSoft
        mBleGatt = mBleDevice.connectGatt(mContext,false,mBleGattCallback);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d("BLE","Service started");
        Toast.makeText(this, "Ble service started",
                Toast.LENGTH_SHORT).show();
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

    private boolean connect(String address) {
        return true;
    }

    private void disconnect(){

    }
}
