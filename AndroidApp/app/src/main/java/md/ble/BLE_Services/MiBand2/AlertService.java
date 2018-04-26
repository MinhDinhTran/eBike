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
public class AlertService<T> extends InfoService<T> {

    private static final String TAG = AlertService.class.getSimpleName();
    /**
     * Service UUID.
     */
    public static final String UUID_SERVICE = "00001802-0000-1000-8000-00805f9b34fb ";

    public static final String UUID_NEW_ALTER_ID = "00002a06-0000-1000-8000-00805f9b34fb";

    /**
     * Characteristics.
     */
    private static final HashMap<String, String> CHARACTERISTIC_MAP = new HashMap<>();

    static {
        CHARACTERISTIC_MAP.put(UUID_NEW_ALTER_ID, "New alter");
    }

    private String _newAlterValue;
    public String getNewAlterValue() {
        return _newAlterValue;
    }

    public AlertService(T model) {
        super(model);
    }

    @Override
    public String getServiceUUID() {
        return UUID_SERVICE;
    }

    @Override
    public String getName() {
        return "Alert Service";
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
            case UUID_NEW_ALTER_ID:
                _newAlterValue = c.getStringValue(0);
                break;
        }
        return true;
    }
}





















