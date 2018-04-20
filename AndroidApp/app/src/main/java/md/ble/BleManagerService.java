package md.ble;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Binder;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.chimeraiot.android.ble.BleManager;
import com.chimeraiot.android.ble.BleService;
import com.chimeraiot.android.ble.BleUtils;
import com.chimeraiot.android.ble.sensor.DeviceDef;
import com.chimeraiot.android.ble.sensor.Sensor;

import md.App;
import md.DB.SensorData;
import md.apk.R;
import md.ble.BLE_Services.BaseDef;
import md.ble.BLE_Services.InfoService;
import md.ble.BLE_Services.MyCustomService;

public class BleManagerService extends com.chimeraiot.android.ble.BleService
        implements ScanProcessor.ScanProcessorListener {
    public enum bleState {
        connected,
        disconnected,
        connectionFailed
    }

    public interface BleStateListener {
        void onBleStateChange(bleState state);
    }

    private static final String TAG = BleManagerService.class.getSimpleName();
    private static final String RECORD_DEVICE_NAME = "BLEPeripheral";
    private ScanProcessor scanner;
    private static final String SENSOR_TO_READ = MyCustomService.UUID_SERVICE;
    private static BleManagerService INSTANCE = null;
    private String adress = null;
    private String deviceName = null;
    private BleStateListener _bleStateListener = null;

    public static BleManagerService getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new BleManagerService();
        }
        return (INSTANCE);
    }

    public void setBleStateListener(BleStateListener listener) {
        _bleStateListener = listener;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        if (INSTANCE == null)
            INSTANCE = this;
        final int bleStatus = BleUtils.getBleStatus(getBaseContext());
        switch (bleStatus) {
            case BleUtils.STATUS_BLE_NOT_AVAILABLE:
                Toast.makeText(getApplicationContext(), R.string.dialog_error_no_ble, Toast.LENGTH_SHORT).show();
                stopSelf();
                return;
            case BleUtils.STATUS_BLUETOOTH_NOT_AVAILABLE:
                Toast.makeText(getApplicationContext(), R.string.dialog_error_no_bluetooth, Toast.LENGTH_SHORT).show();
                stopSelf();
                return;
            default:
                break;
        }
        getBleManager().initialize(getBaseContext());
        final BluetoothAdapter bluetoothAdapter = BleUtils.getBluetoothAdapter(getBaseContext());
        scanner = new ScanProcessor(this, bluetoothAdapter);
    }

    @Override
    protected BleManager createBleManager() {
        return new BleManager(App.DEVICE_DEF_COLLECTION);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (scanner == null)
            return super.onStartCommand(intent, flags, startId);
        scanner.StartScan(RECORD_DEVICE_NAME);
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "Service stopped");
        setServiceListener(null);
        if (scanner != null)
            scanner.Stop();
    }

    @Override
    public void onConnected(final String name, final String address) {
        super.onConnected(name, address);
        if (_bleStateListener != null)
            _bleStateListener.onBleStateChange(bleState.connected);
        //Log.d(TAG, "Connected");
        final InfoService<?> sensor = (InfoService<?>) getBleManager().getDeviceDefCollection()
                .get(name, address).getSensor(SENSOR_TO_READ);
        if (sensor instanceof MyCustomService) {
            getBleManager().listen(address, sensor, MyCustomService.UUID_BIKE_BATTERY_LEVEL_ID);
             getBleManager().listen(address, sensor, MyCustomService.UUID_CURRENT_ID);
        }
    }

    @Override
    public void onDisconnected(final String name, final String address) {
        super.onDisconnected(name, address);
        if (_bleStateListener != null)
            _bleStateListener.onBleStateChange(bleState.disconnected);
        scanner.StartScan(RECORD_DEVICE_NAME);
    }

    @Override
    public void onConnectionFailed(String name, String address, int status, int state) {
        super.onConnectionFailed(name, address, status, state);
        if (_bleStateListener != null)
            _bleStateListener.onBleStateChange(bleState.connectionFailed);
        getBleManager().reset(address);
        scanner.StartScan(RECORD_DEVICE_NAME);
    }

    @Override
    public void onServiceDiscovered(final String name, final String address) {
        super.onServiceDiscovered(name, address);
        // Log.d(TAG, "Service discovered");

    }

    @Override
    public void onCharacteristicChanged(final String name, final String address,
                                        final String serviceUuid,
                                        final String characteristicUuid) {
        super.onCharacteristicChanged(name, address, serviceUuid, characteristicUuid);
        Log.d(TAG, "Service='" + serviceUuid + " characteristic=" + characteristicUuid);
        final InfoService<?> sensor = (InfoService<?>) getBleManager().getDeviceDefCollection()
                .get(name, address).getSensor(SENSOR_TO_READ);
        if (sensor instanceof MyCustomService) {

            try {
                App.sensorDataController.addItemInTable(
                        new SensorData(serviceUuid,
                                characteristicUuid,
                                Integer.toString((((MyCustomService) sensor).getIntValue()))));
            } catch (Exception e) {

            }
        }
    }

    @Override
    public void onAskedDeviceFound(final BluetoothDevice device) {
        adress = device.getAddress();
        deviceName = device.getName();
        getBleManager().connect(getBaseContext(), adress);
    }

    public void update(Sensor<?> sensor, String uuid, Bundle data) {
        if (adress != null)
            getBleManager().update(adress, sensor, uuid, data);
    }


    public DeviceDef getDeviceDef() {
        return new BaseDef<>((Void) null);
       /*if (adress != null && deviceName != null)
            return null;
        return getBleManager().getDeviceDefCollection().get(deviceName, adress);*/
    }
}
