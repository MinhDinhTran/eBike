package md;

import android.app.Application;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.support.annotation.Nullable;

import com.chimeraiot.android.ble.BleConfig;
import com.chimeraiot.android.ble.BleUtils;
import com.chimeraiot.android.ble.sensor.DeviceDef;
import com.chimeraiot.android.ble.sensor.DeviceDefCollection;

import md.DB.SensorDataController;
import md.ble.BLE_Services.BaseDef;
import md.ble.BLE_Services.BaseDef_LeftPedal;
import md.ble.BleManagerService;

/** Application class. */
public class App extends Application {
    private boolean _bleWasEnabled = true;

    public static final DeviceDefCollection DEVICE_DEF_COLLECTION;
    public static final DeviceDef DEVICE_DEF = new BaseDef<>(AppConfig.DEF_EBIKE_PEDAL_R);
    public static final DeviceDef DEVICE_DEF2 = new BaseDef_LeftPedal(AppConfig.DEF_EBIKE_PEDAL_L);
    public static SensorDataController sensorDataController = null;

    static {
        DEVICE_DEF_COLLECTION = new DeviceDefCollection() {
            @Nullable
            @Override
            public DeviceDef create(String name, String address) {
                if (name.equals(AppConfig.DEF_EBIKE_PEDAL_L))
                    return DEVICE_DEF2;
                return DEVICE_DEF;
            }
        };
        DEVICE_DEF_COLLECTION.register(AppConfig.DEF_EBIKE_CENTRAL);
        DEVICE_DEF_COLLECTION.register(AppConfig.DEF_EBIKE_PEDAL_R);
        DEVICE_DEF_COLLECTION.register(AppConfig.DEF_EBIKE_PEDAL_L);
        DEVICE_DEF_COLLECTION.register(AppConfig.DEF_MI_BAND2_DEVICE_NAME);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        BleConfig.setDebugEnabled(true);
        enableBleIfNotEnabled();
        startBleSensorsService();
      //  startGPSService();
    }
    private void startBleSensorsService()
    {
        final BluetoothAdapter adapter = BleUtils.getBluetoothAdapter(this);
        if (adapter == null ) return;

        final Intent gattServiceIntent = new Intent(this, BleManagerService.class);
        if (adapter.isEnabled()) {
             this.startService(gattServiceIntent);
        } else {
            getApplicationContext().stopService(gattServiceIntent);

        }
    }
    private void startGPSService()
    {

        final Intent gattServiceIntent = new Intent(this, GPS_Service.class);

            this.startService(gattServiceIntent);

    }


    private void enableBleIfNotEnabled() {
        // TODO: Ask user if it's ok to enable ble & maybe disable later?
        final BluetoothAdapter adapter = BleUtils.getBluetoothAdapter(this);
        if (adapter != null) {
            _bleWasEnabled = adapter.isEnabled();
            if (!_bleWasEnabled)
                adapter.enable();
        }
    }
}
