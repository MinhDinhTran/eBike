package md.ble;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import com.chimeraiot.android.ble.BleScanner;

import java.util.Set;


public class ScanProcessor implements BleScanner.BleScannerListener {
    public interface ScanProcessorListener {
        void onAskedDeviceFound(BluetoothDevice device, final int rssi);
    }

    private static final long SCAN_PERIOD = 3000L;
    private BleScanner scanner;
    private Set<String> _devicesToFind = null;
    private ScanProcessorListener _listener = null;
    public ScanProcessor(ScanProcessorListener listener, BluetoothAdapter bluetoothAdapter) {
        _listener = listener;
        scanner = new BleScanner(bluetoothAdapter, this);
        scanner.setScanPeriod(SCAN_PERIOD);
    }

    public void FindDevice(Set<String> deviceToFind) {
        _devicesToFind = deviceToFind;
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
        CheckIfAskedDeviceFound(device, rssi);
        //scanMap.put(device, rssi);
    }

    private synchronized void CheckIfAskedDeviceFound(final BluetoothDevice device, final int rssi) {
        if (_devicesToFind == null || _devicesToFind.size() == 0) return;
        if (device == null)
            return;
        final String devName = device.getName();
        if (devName == null)
            return;

        if (_devicesToFind.contains(devName)) {
            if (_listener != null)
                _listener.onAskedDeviceFound(device, rssi);
            _devicesToFind.remove(devName);
            if (_devicesToFind.size() == 0 && scanner.isScanning())
                scanner.stop();
        }
    }
}