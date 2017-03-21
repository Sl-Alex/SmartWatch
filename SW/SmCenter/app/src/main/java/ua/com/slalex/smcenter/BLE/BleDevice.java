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

import ua.com.slalex.smcenter.Constants;

/**
 * A simple wrapper to the HM-10 BLE-to-UART module
 * Created by Sl-Alex on 31.01.2017.
 */

public class BleDevice {

    // For HM-10 read and write characteristic is the same
    private static final String RW_CHARACTERISTIC_ID = "0000FFE1-0000-1000-8000-00805F9B34FB";
    private static final int DEVICE_CONNECT_TIMEOUT = 60000;
    private static final int DEVICE_TRANSFER_TIMEOUT = 500;
    private static final int DEVICE_DISCONNECT_TIMEOUT = 500;
    private static final int DEVICE_RETRIES_MAX = 5;
    private static final int INVALID_RSSI = -255;

    public BleDevice(){
    }

    private int mConnState = BluetoothProfile.STATE_DISCONNECTED;
    private Context mContext = null;
    private String mBtAddress = "";
    private BluetoothDevice mBtDevice = null;
    private BluetoothGatt mBtGatt = null;
    private BluetoothGattCallback mBtGattCallback = null;
    private BluetoothGattCharacteristic mRwCharacteristic = null;
    private byte[] mDataIn;
    private int mRssi;

    private CountDownLatch mConnectLatch = null;
    private CountDownLatch mTransferLatch = null;
    private CountDownLatch mDisconnectLatch = null;

    public void setBtAddress(String address, Context context) {
        mContext = context;

        if (mBtAddress.equals(address))
            return;

        if (mConnState == BluetoothProfile.STATE_CONNECTED)
            disconnect();

        mBtAddress = address;
    }

    private void init() {
        // Check and disconnect if connected
        if (mConnState == BluetoothProfile.STATE_CONNECTED)
            disconnect();

        BluetoothManager manager = (BluetoothManager)mContext.getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter adapter = manager.getAdapter();
        mBtDevice = null;
        if (adapter == null)
        {
            return;
        }
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
                    /// TODO: Replace with constant
                    mRssi = INVALID_RSSI;
                    gatt.readRemoteRssi();
                }
                if (newState == BluetoothProfile.STATE_DISCONNECTED)
                {
                    mConnState = newState;
                    if (mDisconnectLatch != null)
                        mDisconnectLatch.countDown();
                    if (mConnectLatch != null)
                        mConnectLatch.countDown();
                }
            }

            @Override
            public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
                super.onReadRemoteRssi(gatt, rssi, status);
                mRssi = rssi;
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": mRssi = " + mRssi);
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
                                gatt.close();
                            }
                            break;
                        }
                    }
                }
            }
        };
        mBtDevice = adapter.getRemoteDevice(mBtAddress);
    }

    boolean connect() {
        if (mBtDevice == null)
            init();

        if (mBtDevice == null)
            return false;

        if (mConnState == BluetoothProfile.STATE_CONNECTED)
            return true;

        // Wait for the result with a timeout
        boolean ret = false;

        int retries = DEVICE_RETRIES_MAX;
        while (retries > 0)
        {
            mConnectLatch = new CountDownLatch(1);

            mBtGatt = mBtDevice.connectGatt(mContext,false,mBtGattCallback);

            try {
                ret = mConnectLatch.await(DEVICE_CONNECT_TIMEOUT, TimeUnit.MILLISECONDS);
            } catch (InterruptedException e) {
            }

            // Didn't connect, disconnecting anyway
            if (mConnState != BluetoothProfile.STATE_CONNECTED)
            {
                ret = false;
                disconnect();
            }

            if (ret) {
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "connected");
                break;
            } else {
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "not connected");
                retries--;
            }
        }

        if (!ret) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Connect timeout");
        }

        // Invalidate mConnectLatch
        mConnectLatch = null;

        return ret;
    }

    public boolean disconnect() {
        if (mBtDevice == null)
            return true;

        mDisconnectLatch = new CountDownLatch(1);

        if (mBtGatt != null)
            mBtGatt.disconnect();

        mConnState = BluetoothProfile.STATE_DISCONNECTED;

        // Wait for the result with a timeout
        boolean ret = false;

        try {
            ret = mDisconnectLatch.await(DEVICE_DISCONNECT_TIMEOUT, TimeUnit.MILLISECONDS);
        } catch (InterruptedException e) {
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "InterruptedException");
        }

        if (!ret)
            Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "Disconnect timeout");

        if (mBtGatt != null)
            mBtGatt.close();

        // Invalidate mDisconnectLatch
        mDisconnectLatch = null;

        return ret;
    }

    byte[] transfer(byte[] data) {
        if (mConnState != BluetoothProfile.STATE_CONNECTED)
            return null;

        if (mBtDevice == null)
            return null;

        mDataIn = null;

        // Wait for the result with a timeout
        boolean ret = false;

        int retries = DEVICE_RETRIES_MAX;
        while (retries > 0)
        {
            mTransferLatch = new CountDownLatch(1);
            mRwCharacteristic.setValue(data);
            mBtGatt.writeCharacteristic(mRwCharacteristic);

            try {
                ret = mTransferLatch.await(DEVICE_TRANSFER_TIMEOUT, TimeUnit.MILLISECONDS);
            } catch (InterruptedException e) {
                disconnect();
                break;
            }
            if (ret) {
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "ret = true");
                break;
            } else {
                Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + "ret = false");
                retries--;
            }
        }

        if (!ret) {
            disconnect();
        }

        // Invalidate mTransferLatch
        mTransferLatch = null;

        return mDataIn;
    }

    int getRssi() {
        return mRssi;
    }
}
