package md.ble.BLE_Services.MiBand2;

import android.bluetooth.BluetoothGattCharacteristic;
import android.os.Bundle;

import com.chimeraiot.android.ble.BleGattExecutor;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.HashMap;

import md.ble.BLE_Services.BLEConst;
import md.ble.BLE_Services.InfoService;


/** BLE device info services. */
public class BasicService<T> extends InfoService<T> {


    private static final String TAG = BasicService.class.getSimpleName();
    /**
     * Service UUID.
     */
    public static final String UUID_SERVICE = "0000fee0-0000-1000-8000-00805f9b34fb";

    public static final String UUID_BATTERY_INFO_ID = "00000006-0000-3512-2118-0009af100700";

    /**
     * Characteristics.
     */
    private static final HashMap<String, String> CHARACTERISTIC_MAP = new HashMap<>();

    static {
        CHARACTERISTIC_MAP.put(UUID_BATTERY_INFO_ID, "Battery Info");
    }

    private String _batteryInfoValue;
    public String getBatteryInfo() {
        return _batteryInfoValue;
    }



    public BasicService(T model) {
        super(model);
    }

    @Override
    public String getServiceUUID() {
        return UUID_SERVICE;
    }

    @Override
    public String getName() {
        return "Basic Service";
    }

    @Override
    public String getCharacteristicName(String uuid) {
        if (!CHARACTERISTIC_MAP.containsKey(uuid)) {
            return "Unknown";
        }
        return CHARACTERISTIC_MAP.get(uuid);
    }


    @Override
    protected boolean apply(final BluetoothGattCharacteristic c, final T data) {
        switch (c.getUuid().toString()) {
            case UUID_BATTERY_INFO_ID:
                _batteryInfoValue = c.getStringValue( 0);
                break;
        }
        return true;
    }
}





















