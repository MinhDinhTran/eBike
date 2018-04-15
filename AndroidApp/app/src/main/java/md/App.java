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
import md.ble.BleManagerService;

/** Application class. */
public class App extends Application {
    private boolean _bleWasEnabled = true;


    public static final DeviceDefCollection DEVICE_DEF_COLLECTION;
    public static SensorDataController sensorDataController = null;
    static {
        DEVICE_DEF_COLLECTION = new DeviceDefCollection() {
            @Nullable
            @Override
            public DeviceDef create(String name, String address) {
                return new BaseDef<>((Void)null);
            }
        };
        DEVICE_DEF_COLLECTION.register(AppConfig.DEF_DEVICE_NAME);
    }


    @Override
    public void onCreate() {
        super.onCreate();
        BleConfig.setDebugEnabled(true);
        enableBleIfNotEnabled();
        startBleSensorsService(true);
    }
    private void startBleSensorsService(boolean start)
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
