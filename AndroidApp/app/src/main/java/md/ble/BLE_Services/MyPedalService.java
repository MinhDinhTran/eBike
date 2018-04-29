package md.ble.BLE_Services;

import android.bluetooth.BluetoothGattCharacteristic;
import android.os.Bundle;

import com.chimeraiot.android.ble.BleGattExecutor;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.HashMap;


/** BLE device info services. */
public class MyPedalService<T> extends InfoService<T> {
    public interface MyPedalServiceListener {
        void OnCharacteristicChanged(String characteristic, int value);
        void OnCharacteristicChanged(String characteristic, float value);
    }

    private static final String TAG = MyPedalService.class.getSimpleName();
    /**
     * Service UUID.
     */
    public static final String UUID_SERVICE = "0000ffe0-0000-1000-8000-00805f9b34fb";

    public static final String UUID_BATTERY_LV_ID = "0000ffe1-0000-1000-8000-00805f9b34fb";
    public static final String UUID_PEDALLING_STR_ID = "0000ffe2-0000-1000-8000-00805f9b34fb";
    public static final String UUID_CONFIG_ID = "0000ffe3-0000-1000-8000-00805f9b34fb";
    public static final String UUID_RAW_DATA_ID = "0000ffef-0000-1000-8000-00805f9b34fb";

    /**
     * Characteristics.
     */
    private static final HashMap<String, String> CHARACTERISTIC_MAP = new HashMap<>();

    static {
        CHARACTERISTIC_MAP.put(UUID_BATTERY_LV_ID, "UUID_BATTERY_LV_ID");
        CHARACTERISTIC_MAP.put(UUID_PEDALLING_STR_ID, "UUID_PEDALLING_STR_ID");
        CHARACTERISTIC_MAP.put(UUID_CONFIG_ID, "UUID_CONFIG_ID");
        CHARACTERISTIC_MAP.put(UUID_RAW_DATA_ID, "UUID_RAW_DATA_ID");
    }

    private int _rawValue;

    public int getRawValue() {
        return _rawValue;
    }

    private MyPedalServiceListener _listener = null;

    public void setServiceListener(MyPedalServiceListener listener) {
        _listener = listener;
    }

    protected MyPedalService(T model) {
        super(model);
    }

    @Override
    public String getServiceUUID() {
        return UUID_SERVICE;
    }

    @Override
    public String getName() {
        return "pedal service";
    }

    @Override
    public String getCharacteristicName(String uuid) {
        if (!CHARACTERISTIC_MAP.containsKey(uuid)) {
            return "Unknown";
        }
        return CHARACTERISTIC_MAP.get(uuid);
    }

    @Override
    public BleGattExecutor.ServiceAction[] update(final String uuid, final Bundle data) {
        byte[] bytes = {0, 0, 0, 0};

        try {
            bytes = ByteBuffer.allocate(4)
                    .order(ByteOrder.LITTLE_ENDIAN)
                    .putInt(Integer.parseInt(data.get(BLEConst.DATA).toString()))
                    .array();
        } catch (Exception ex) {
        }

        switch (uuid) {
            case UUID_CONFIG_ID:
                return new BleGattExecutor.ServiceAction[]{
                        write(UUID_CONFIG_ID, bytes)
                };
            default:
                return super.update(uuid, data);
        }
    }


    @Override
    protected boolean apply(final BluetoothGattCharacteristic c, final T data) {
        int intValue = 0;
        float floatValue = 0;
        switch (c.getUuid().toString()) {
            case UUID_BATTERY_LV_ID:
                intValue = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0);
                break;
            case UUID_PEDALLING_STR_ID:
                _rawValue = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT32, 0);
                break;
            case UUID_RAW_DATA_ID:
                intValue = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0);
                //floatValue = c.getFloatValue(BluetoothGattCharacteristic.FORMAT_FLOAT, 0);
                break;
        }
       /* if (_listener != null) {
            if (!c.getUuid().toString().equals( UUID_PEDALLING_STR_ID))
                _listener.OnCharacteristicChanged(c.getUuid().toString(), intValue);
            else
                _listener.OnCharacteristicChanged(c.getUuid().toString(), floatValue);
        }*/
        return true;
    }
}





















