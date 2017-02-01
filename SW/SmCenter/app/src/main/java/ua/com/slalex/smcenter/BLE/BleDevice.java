package ua.com.slalex.smcenter.BLE;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.util.Log;

import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import static android.bluetooth.BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT;

/**
 * Created by Sl-Alex on 31.01.2017.
 */

public class BleDevice {

    // For HM-10 read and write characteristic is the same
    private static final String RW_CHARACTERISTIC_ID = "0000FFE1-0000-1000-8000-00805F9B34FB";
    private static final int DEVICE_CONNECT_TIMEOUT = 5000;
    private static final int DEVICE_TRANSFER_TIMEOUT = 500;
    private static final int DEVICE_DISCONNECT_TIMEOUT = 500;

    public BleDevice(){
    }

    int mConnState = BluetoothProfile.STATE_DISCONNECTED;
    String mAddress = "";
    Context mContext = null;
    BluetoothManager mBtManager = null;
    BluetoothAdapter mBtAdapter = null;
    BluetoothDevice mBtDevice = null;
    BluetoothGatt mBtGatt = null;
    BluetoothGattCallback mBtGattCallback = null;
    BluetoothGattCharacteristic mRwCharacteristic = null;

    private CountDownLatch mConnectLatch = null;
    private CountDownLatch mTransferLatch = null;
    private CountDownLatch mDisconnectLatch = null;

    public void init(String address, Context context) {
        // Check and disconnect if connected
        if (mConnState == BluetoothProfile.STATE_CONNECTED)
            disconnect();

        mAddress = address;
        mContext = context;
        mBtManager = (BluetoothManager)mContext.getSystemService(Context.BLUETOOTH_SERVICE);
        mBtAdapter = mBtManager.getAdapter();
        mBtGattCallback = new BluetoothGattCallback() {
            @Override
            public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
                super.onCharacteristicChanged(gatt, characteristic);
                Log.d("BLE","Characteristic changed");
            }

            @Override
            public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
                super.onConnectionStateChange(gatt, status, newState);
                if (newState == BluetoothProfile.STATE_CONNECTED)
                {
                    gatt.discoverServices();
                }
                if (newState == BluetoothProfile.STATE_DISCONNECTED)
                {
                    mConnState = newState;
                    if (mDisconnectLatch != null)
                        mDisconnectLatch.countDown();
                }
            }

            @Override
            public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
                super.onCharacteristicWrite(gatt, characteristic, status);
                if (mTransferLatch != null)
                {
                    if (status == BluetoothGatt.GATT_SUCCESS)
                    {
                        mTransferLatch.countDown();
                    }
                }
            }


            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                super.onServicesDiscovered(gatt, status);
                if (status != BluetoothGatt.GATT_SUCCESS)
                    return;
                List<BluetoothGattService> services = gatt.getServices();
                for (BluetoothGattService service : services) {
                    List<BluetoothGattCharacteristic> characteristics = service.getCharacteristics();
                    for (BluetoothGattCharacteristic characteristic : characteristics) {
                        if (characteristic.getUuid().toString().equalsIgnoreCase(RW_CHARACTERISTIC_ID))
                        {
                            gatt.setCharacteristicNotification(characteristic, true);
                            mConnState = BluetoothProfile.STATE_CONNECTED;
                            mRwCharacteristic = characteristic;
                            mRwCharacteristic.setWriteType(WRITE_TYPE_DEFAULT);
                            if (mConnectLatch != null) {
                                mRwCharacteristic = characteristic;
                                mConnectLatch.countDown();
                            }
                            else {
                                gatt.disconnect();
                            }
                            break;
                        }
                    }
                }
            }
        };
        mBtDevice = mBtAdapter.getRemoteDevice(mAddress);
    }

    public boolean disconnect() {
        if (mConnState == BluetoothProfile.STATE_DISCONNECTED)
            return true;

        mDisconnectLatch = new CountDownLatch(1);

        mBtGatt.disconnect();
        mConnState = BluetoothProfile.STATE_DISCONNECTED;

        // Wait for the result with a timeout
        boolean ret = false;

        try {
            ret = mDisconnectLatch.await(DEVICE_DISCONNECT_TIMEOUT, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
        }

        if (ret == false) {
            Log.d("BLE", "Disconnect timeout");
        } else {
            Log.d("BLE", "Disconnected");
        }

        // Invalidate mDisconnectLatch
        mDisconnectLatch = null;

        return ret;
    }

    public boolean connect() {
        if (mConnState == BluetoothProfile.STATE_CONNECTED)
            return true;

        mConnectLatch = new CountDownLatch(1);

        mBtGatt = mBtDevice.connectGatt(mContext,false,mBtGattCallback);

        // Wait for the result with a timeout
        boolean ret = false;

        try {
            ret = mConnectLatch.await(DEVICE_CONNECT_TIMEOUT, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            disconnect();
        }

        if (ret == false) {
            Log.d("BLE", "Connect timeout");
            // Didn't connect, disconnecting anyway
            disconnect();
        } else {
            Log.d("BLE", "Connected");
        }

        // Invalidate mConnectLatch
        mConnectLatch = null;

        return ret;
    }

    public boolean writeData(byte[] data) {
        if (mConnState != BluetoothProfile.STATE_CONNECTED)
            return false;

        mTransferLatch = new CountDownLatch(1);

        mRwCharacteristic.setValue(data);
        mBtGatt.writeCharacteristic(mRwCharacteristic);

        // Wait for the result with a timeout
        boolean ret = false;

        try {
            ret = mTransferLatch.await(DEVICE_TRANSFER_TIMEOUT, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            disconnect();
        }

        if (ret == false) {
            Log.d("BLE", "Transfer timeout");
            // Didn't transfer, disconnecting anyway
            /// TODO: Implement resending
            disconnect();
        } else {
            Log.d("BLE", "Transfer done");
        }

        // Invalidate mTransferLatch
        mTransferLatch = null;

        return ret;
    }
}
