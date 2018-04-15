package md.ble.BLE_Services;

import android.bluetooth.BluetoothGattCharacteristic;
import android.os.Bundle;
import android.util.Log;

import com.chimeraiot.android.ble.BleGattExecutor;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.HashMap;


/** BLE device info services. */
public class MyCustomService<T> extends InfoService<T> {

    private static final String TAG = MyCustomService.class.getSimpleName();
    /** Service UUID. */
    public static final String UUID_SERVICE = "0000fff0-0000-1000-8000-00805f9b34fb";

    public static final String UUID_MODE_ID = "0000fff1-0000-1000-8000-00805f9b34fb";
    public static final String UUID_PWM_DUTY_CYCLE_ID = "0000fff2-0000-1000-8000-00805f9b34fb";
    public static final String UUID_V_THRESHOLD_ID = "0000fff3-0000-1000-8000-00805f9b34fb";
    public static final String UUID_BIKE_BATTERY_LEVEL_ID = "0000fff4-0000-1000-8000-00805f9b34fb";
    public static final String UUID_CURRENT_ID = "0000fff5-0000-1000-8000-00805f9b34fb";

    /** Characteristics. */
    private static final HashMap<String, String> CHARACTERISTIC_MAP = new HashMap<>();

    static {
        CHARACTERISTIC_MAP.put(UUID_MODE_ID, "Working mode");
        CHARACTERISTIC_MAP.put(UUID_PWM_DUTY_CYCLE_ID, "PWM duty cycle");
        CHARACTERISTIC_MAP.put(UUID_V_THRESHOLD_ID, "V THRESHOLD");
        CHARACTERISTIC_MAP.put(UUID_BIKE_BATTERY_LEVEL_ID, "Bike battery level");
        CHARACTERISTIC_MAP.put(UUID_CURRENT_ID, "Electric current level");
    }

    private int value;
    public int getIntValue() {
        return value;
    }

    protected MyCustomService(T model) {
        super(model);
    }

    @Override
    public String getServiceUUID() {
        return UUID_SERVICE;
    }

    @Override
    public String getName() {
        return "My custom service";
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
    byte[] bytes = {0,0,0,0};

    try {
         bytes = ByteBuffer.allocate(4)
                 .order(ByteOrder.LITTLE_ENDIAN)
                .putInt(Integer.parseInt(data.get(BLEConst.DATA).toString()))
                .array();
    }
    catch (Exception ex){}

        switch (uuid) {
            case UUID_PWM_DUTY_CYCLE_ID:
                return new BleGattExecutor.ServiceAction[]{
                        write(UUID_PWM_DUTY_CYCLE_ID, new byte[]{bytes[0]})
                };
            case UUID_MODE_ID:
                return new BleGattExecutor.ServiceAction[]{
                        write(UUID_MODE_ID, new byte[]{bytes[0]})
                };
            case UUID_V_THRESHOLD_ID:
                return new BleGattExecutor.ServiceAction[]{
                        write(UUID_V_THRESHOLD_ID, bytes)
                };
            default:
                return super.update(uuid, data);
        }
    }


    @Override
    protected boolean apply(final BluetoothGattCharacteristic c, final T data) {
        switch (c.getUuid().toString())
        {
            case UUID_BIKE_BATTERY_LEVEL_ID:
                value = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT32, 0);
                break;
            case UUID_CURRENT_ID:
                value = c.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT32, 0);
                break;
        }

        Log.d(TAG, "MyCustomService.Value ='" + value);
        return true;
    }
}
























