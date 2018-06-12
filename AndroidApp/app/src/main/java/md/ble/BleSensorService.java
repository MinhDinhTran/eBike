package md.ble;

        import com.chimeraiot.android.ble.BleManager;

        import md.App;

/** BLE service. */
public class BleSensorService extends com.chimeraiot.android.ble.BleService {

    @Override
    protected BleManager createBleManager() {
        return new BleManager(App.DEVICE_DEF_COLLECTION);
    }
}
