package md.ble;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattService;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import com.chimeraiot.android.ble.BleManager;
import com.chimeraiot.android.ble.BleUtils;
import com.chimeraiot.android.ble.sensor.Sensor;

import Loging.FileLog;
import md.App;
import md.AppConfig;
import md.DB.SensorData;
import md.apk.R;
import md.ble.BLE_Services.InfoService;
import md.ble.BLE_Services.MiBand2.HeartRateService;
import md.ble.BLE_Services.MyCustomService;
import md.ble.BLE_Services.MyPedalService;

public class BleManagerService extends com.chimeraiot.android.ble.BleService
        implements ScanProcessor.ScanProcessorListener {
    public enum bleState {
        connected,
        disconnected,
        connectionFailed
    }
    public interface BleStateListener {
        void onBleStateChange(final String name, final bleState state);
    }

    private class FoundDeviceEntity {
        private final String _adress;
        private final String _deviceName;
        private int _rssi;

        //private bleState _state = bleState.disconnected;
        //private final BluetoothDevice _device;
        public FoundDeviceEntity(final BluetoothDevice device, final int rssi) {
            //    _device = device;
            _adress = device.getAddress();
            _deviceName = device.getName();
            _rssi = rssi;
        }

        public String getAdress() {
            return _adress;
        }

        public String getDeviceName() {
            return _deviceName;
        }

        public int getRssi() {
            return _rssi;
        }

        public void setRssi(final int rssi) {
            _rssi = rssi;
        }
    }
    private static final String TAG = BleManagerService.class.getSimpleName();
    //private static final Set<String> RECORD_DEVICE_NAME = new HashSet<>(Arrays.asList(
   //         new String[]{AppConfig.DEF_EBIKE_CENTRAL, AppConfig.DEF_EBIKE_PEDAL_R/*, AppConfig.DEF_MI_BAND2_DEVICE_NAME*/}
    //));
    private ScanProcessor scanner = null;
    private static BleManagerService INSTANCE = null;

    private BleStateListener _bleStateListener = null;

    public static BleManagerService getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new BleManagerService();
        }
        return (INSTANCE);
    }

    public void setBleStateListener(BleStateListener listener) {
        _bleStateListener = listener;
        if (_bleStateListener != null) {
            for (BluetoothDevice device : getBleManager().getConnectedDevices()) {
                _bleStateListener.onBleStateChange(device.getName(), bleState.connected);
            }
        }
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
        //scanner.FindDevice(RECORD_DEVICE_NAME);
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
            _bleStateListener.onBleStateChange(name, bleState.connected);
    }

    @Override
    public void onDisconnected(final String name, final String address) {
        super.onDisconnected(name, address);
        if (_bleStateListener != null)
            _bleStateListener.onBleStateChange(name, bleState.disconnected);
        getBleManager().reset(address);
        //scanner.FindDevice(RECORD_DEVICE_NAME);
    }

    @Override
    public void onConnectionFailed(String name, String address, int status, int state) {
        super.onConnectionFailed(name, address, status, state);
        if (_bleStateListener != null)
            _bleStateListener.onBleStateChange(name, bleState.connectionFailed);
        getBleManager().reset(address);
        //scanner.FindDevice(RECORD_DEVICE_NAME);
    }

    @Override
    public void onServiceDiscovered(final String name, final String address) {
        super.onServiceDiscovered(name, address);
        Log.d(TAG, "Service discovered_______");

        for ( BluetoothGattService service : getBleManager().getSupportedGattServices(address)){
            if (service.getUuid().toString().equals(MyCustomService.UUID_SERVICE))
            {
                final InfoService<?> sensor = (InfoService<?>) getBleManager().getDeviceDefCollection()
                        .get(name, address).getSensor(MyCustomService.UUID_SERVICE);
                if (sensor != null) {
                    getBleManager().listen(address, sensor, MyCustomService.UUID_BIKE_BATTERY_LEVEL_ID);
                    getBleManager().listen(address, sensor, MyCustomService.UUID_CURRENT_ID);
                    getBleManager().listen(address, sensor, MyCustomService.UUID_BIKE_SPEED_ID);
                    getBleManager().listen(address, sensor, MyCustomService.UUID_BIKE_FLAGS_ID);
                }
            }
            if (service.getUuid().toString().equals(HeartRateService.UUID_SERVICE)) {
                final InfoService<?> sensor = (InfoService<?>) getBleManager().getDeviceDefCollection()
                        .get(name, address).getSensor(HeartRateService.UUID_SERVICE);

                if (sensor != null) {
                    getBleManager().listen(address, sensor, HeartRateService.UUID_HEARTRATE_MEASURE_ID);
                }
            }
            if (service.getUuid().toString().equals(MyPedalService.UUID_SERVICE)) {
                final InfoService<?> sensor = (InfoService<?>) getBleManager().getDeviceDefCollection()
                        .get(name, address).getSensor(MyPedalService.UUID_SERVICE);

                if (sensor != null) {
                    getBleManager().listen(address, sensor, MyPedalService.UUID_BATTERY_LV_ID);
                    getBleManager().listen(address, sensor, MyPedalService.UUID_PEDALLING_STR_ID);
                    getBleManager().listen(address, sensor, MyPedalService.UUID_RAW_DATA_ID);
                }
            }
        }
    }

    @Override
    public void onCharacteristicChanged(final String name, final String address,
                                        final String serviceUuid,
                                        final String characteristicUuid) {
        super.onCharacteristicChanged(name, address, serviceUuid, characteristicUuid);

        SensorData sensorData = new SensorData(serviceUuid, characteristicUuid);

        final InfoService<?> sensor = (InfoService<?>) getBleManager().getDeviceDefCollection()
                .get(name, address).getSensor(serviceUuid);

        if (sensor instanceof MyCustomService) {
            if (characteristicUuid.equals(MyCustomService.UUID_BIKE_BATTERY_LEVEL_ID))
                sensorData.setValue((((MyCustomService) sensor).getBatValue()));
            else if (characteristicUuid.equals(MyCustomService.UUID_BIKE_SPEED_ID))
                sensorData.setValue((((MyCustomService) sensor).getRPMValue()));
            else if (characteristicUuid.equals(MyCustomService.UUID_CURRENT_ID))
                sensorData.setValue((((MyCustomService) sensor).getCurrentValue()));
            else if (characteristicUuid.equals(MyCustomService.UUID_PWM_DUTY_CYCLE_ID))
                sensorData.setValue((((MyCustomService) sensor).getDutyCycleValue()));

        }
        else if (sensor instanceof HeartRateService)
            sensorData.setValue(((HeartRateService) sensor).getMeasureValue());
        else if (sensor instanceof MyPedalService)
            sensorData.setValue((((MyPedalService) sensor).getRawValue()));

        try {
            if (!serviceUuid.equals(MyPedalService.UUID_BATTERY_LV_ID))
                FileLog.Write("log_ "+name + characteristicUuid.substring(4,8), sensorData.toString());
            //App.sensorDataController.addItemInTable(sensorData);
        } catch (Exception e) {
            Log.d(TAG, "onCharacteristicChanged: exception " + e.getMessage());
        }
    }

    @Override
    public void onAskedDeviceFound(final BluetoothDevice device, final int rssi) {
        final FoundDeviceEntity foundDevice = new FoundDeviceEntity(device, rssi);
        //_foundDevices.add(foundDevice);
        String address = foundDevice.getAdress();
        boolean found = false;
        for (BluetoothDevice dev : getBleManager().getConnectedDevices()) {
            if (dev.getAddress().equals(address)) {
                found = true;
                break;
            }
        }
        if (!found)
            getBleManager().connect(getBaseContext(), address);
    }

    public void update(Sensor<?> sensor, String uuid, Bundle data) {
        for ( BluetoothDevice device : getBleManager().getConnectedDevices()){
            if (sensor instanceof HeartRateService && device.getName().equals(AppConfig.DEF_MI_BAND2_DEVICE_NAME))
            {
                getBleManager().update(device.getAddress(), sensor, uuid, data);
            }
            else if  (sensor instanceof MyCustomService && device.getName().equals(AppConfig.DEF_EBIKE_CENTRAL))
            {
                getBleManager().update(device.getAddress(), sensor, uuid, data);
            }
            else if  (sensor instanceof MyPedalService && device.getName().equals(AppConfig.DEF_EBIKE_PEDAL_R))
            {
                getBleManager().update(device.getAddress(), sensor, uuid, data);
            }
            else if  (sensor instanceof MyPedalService && device.getName().equals(AppConfig.DEF_EBIKE_PEDAL_L))
            {
                getBleManager().update(device.getAddress(), sensor, uuid, data);
            }
            else
            {
                Log.wtf(TAG, "wtf: update " + uuid);
            }
        }
    }
    private BluetoothDevice getConnectedDev(String name) {
        for (BluetoothDevice device : getBleManager().getConnectedDevices()) {
            if (device.getName().equals(name))
                return device;
        }
        return null;
    }
    public void ConnectToDevice(String name)
    {
        scanner.FindDevice(name);
    }
    public void CancelSearch(String name) {
        BluetoothDevice dev = getConnectedDev(name);
        if (dev != null) {
            getBleManager().reset(dev.getAddress());
            if (_bleStateListener != null)
                _bleStateListener.onBleStateChange(name, bleState.disconnected);
        }
        else
            scanner.CancelSearch(name);
    }
}
