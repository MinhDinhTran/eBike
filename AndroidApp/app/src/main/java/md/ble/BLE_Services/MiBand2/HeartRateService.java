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
public class HeartRateService<T> extends InfoService<T> {
    public interface HeartRateServiceListener {
        void OnCharacteristicChanged(String characteristic, int value);
    }

    private static final String TAG = HeartRateService.class.getSimpleName();
    /**
     * Service UUID.
     */
    public static final String UUID_SERVICE = "0000180d-0000-1000-8000-00805f9b34fb";

    public static final String UUID_HEARTRATE_MEASURE_ID = "00002a37-0000-1000-8000-00805f9b34fb";
    public static final String UUID_HEARTRATE_CONTROL_ID = "00002a39-0000-1000-8000-00805f9b34fb";

    /**
     * Characteristics.
     */
    private static final HashMap<String, String> CHARACTERISTIC_MAP = new HashMap<>();

    static {
        CHARACTERISTIC_MAP.put(UUID_HEARTRATE_MEASURE_ID, "Measure");
        CHARACTERISTIC_MAP.put(UUID_HEARTRATE_CONTROL_ID, "Control");
    }

    private int _measureValue;

    public int getMeasureValue() {
        return _measureValue;
    }

    private String _controlValue;

    public String getControlValue() {
        return _controlValue;
    }

    private HeartRateServiceListener _listener = null;
    public void setServiceListener(HeartRateServiceListener listener) {
        _listener = listener;
    }

    public HeartRateService(T model) {
        super(model);
    }

    @Override
    public String getServiceUUID() {
        return UUID_SERVICE;
    }

    @Override
    public String getName() {
        return "Heart Rate Service";
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
        switch (uuid) {
            case UUID_HEARTRATE_CONTROL_ID:
                return new BleGattExecutor.ServiceAction[]{
                        write(UUID_HEARTRATE_CONTROL_ID, new byte[]{21, 2, 1})
                };
            default:
                return super.update(uuid, data);
        }
    }


    @Override
    protected boolean apply(final BluetoothGattCharacteristic c, final T data) {
        switch (c.getUuid().toString()) {
            case UUID_HEARTRATE_MEASURE_ID:
                byte bytes[] = c.getValue();
                _measureValue = (bytes[0] << 8) + bytes[1];
                if (_listener != null)
                    _listener.OnCharacteristicChanged(UUID_HEARTRATE_MEASURE_ID, _measureValue);
                break;
            case UUID_HEARTRATE_CONTROL_ID:
                _controlValue = c.getStringValue(0);
                break;
        }
        return true;
    }
}





















