package md.ble;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.util.Log;

import com.chimeraiot.android.ble.BleScanner;
import java.util.HashMap;
import java.util.Map;


public class ScanProcessor implements BleScanner.BleScannerListener {
    public interface ScanProcessorListener {
        void onAskedDeviceFound(BluetoothDevice device);
    }

    private static final long SCAN_PERIOD = 3000L;
    private BleScanner scanner;
    //private final Map<BluetoothDevice, Integer> scanMap = new HashMap<>();
    private String _deviceToFind = "";
    private ScanProcessorListener _listener = null;
    private boolean _found = true;
    public ScanProcessor(ScanProcessorListener listener, BluetoothAdapter bluetoothAdapter) {
        _listener = listener;
        scanner = new BleScanner(bluetoothAdapter, this);
        scanner.setScanPeriod(SCAN_PERIOD);
    }

    public void StartScan(String deviceToFind) {
        _deviceToFind = deviceToFind;
        _found = false;
        scanner.start();
    }
    public void Stop() {
        scanner.stop();
    }

    @Override
    public void onScanStarted() {

    }

    @Override
    public void onScanRepeat() {

    }

    @Override
    public void onScanStopped() {

    }

    @Override
    public void onLeScan(final BluetoothDevice device, final int rssi,
                         byte[] bytes) {
        if (device == null) return;
        CheckIfAskedDeviceFound(device);
        //scanMap.put(device, rssi);
    }

    private synchronized void CheckIfAskedDeviceFound(final BluetoothDevice device) {
        if (_found) return;
        if (device == null)
            return;
        final String devName = device.getName();
        if (devName == null)
            return;

        if (devName.equals(_deviceToFind)) {
            if (_listener != null)
                _listener.onAskedDeviceFound(device);
            if (scanner.isScanning())
                scanner.stop();
            _found = true;
        }
    }
}