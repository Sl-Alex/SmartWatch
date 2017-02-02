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

/**
 * A simple wrapper to the HM-10 BLE-to-UART module
 * Created by Sl-Alex on 31.01.2017.
 */

public class BleDevice {

    // For HM-10 read and write characteristic is the same
    private static final String RW_CHARACTERISTIC_ID = "0000FFE1-0000-1000-8000-00805F9B34FB";
    private static final int DEVICE_CONNECT_TIMEOUT = 5000;
    private static final int DEVICE_TRANSFER_TIMEOUT = 5000;
    private static final int DEVICE_DISCONNECT_TIMEOUT = 500;

    public BleDevice(){
    }

    private int mConnState = BluetoothProfile.STATE_DISCONNECTED;
    private Context mContext = null;
    private BluetoothDevice mBtDevice = null;
    private BluetoothGatt mBtGatt = null;
    private BluetoothGattCallback mBtGattCallback = null;
    private BluetoothGattCharacteristic mRwCharacteristic = null;
    private byte[] mDataIn;

    private CountDownLatch mConnectLatch = null;
    private CountDownLatch mTransferLatch = null;
    private CountDownLatch mDisconnectLatch = null;

    public void init(String address, Context context) {
        // Check and disconnect if connected
        if (mConnState == BluetoothProfile.STATE_CONNECTED)
            disconnect();

        mContext = context;
        BluetoothManager manager = (BluetoothManager)mContext.getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter adapter = manager.getAdapter();
        mBtGattCallback = new BluetoothGattCallback() {
            @Override
            public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
                super.onCharacteristicChanged(gatt, characteristic);
                mDataIn = characteristic.getValue();
                if (mTransferLatch != null)
                {
                    mTransferLatch.countDown();
                }
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
                // Write is done
                super.onCharacteristicWrite(gatt, characteristic, status);
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
                            mRwCharacteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
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
        mBtDevice = adapter.getRemoteDevice(address);
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

        // Didn't connect, disconnecting anyway
        if (!ret) {
            Log.d("BLE", "Connect timeout");
            disconnect();
        }

        // Invalidate mConnectLatch
        mConnectLatch = null;

        return ret;
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
            Log.d("BLE", "InterruptedException");
        }

        if (!ret)
            Log.d("BLE", "Disconnect timeout");

        mBtGatt.close();

        // Invalidate mDisconnectLatch
        mDisconnectLatch = null;

        return ret;
    }

    public byte[] transfer(byte[] data) {
        if (mConnState != BluetoothProfile.STATE_CONNECTED)
            return null;

        mDataIn = null;
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

        if (!ret) {
            Log.d("BLE", "Transfer timeout");
            /// TODO: Implement resending
            disconnect();
        }

        // Invalidate mTransferLatch
        mTransferLatch = null;

        return mDataIn;
    }
}
