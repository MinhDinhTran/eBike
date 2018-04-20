package md.ble.ui;

import android.app.Activity;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.text.TextUtils;
import android.view.MenuItem;
import android.widget.ExpandableListView;
import android.widget.SeekBar;
import android.widget.TextView;

import com.chimeraiot.android.ble.BleService;
import com.chimeraiot.android.ble.BleServiceBindingActivity;
import com.chimeraiot.android.ble.sensor.DeviceDef;
import com.chimeraiot.android.ble.sensor.Sensor;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;

import java.util.ArrayList;
import java.util.List;

import md.App;
import md.ble.BLE_Services.BLEConst;
import md.ble.BLE_Services.InfoService;
import md.ble.BLE_Services.MyCustomService;
import md.ble.BleManagerService;
import md.ble.BleSensorService;
import md.apk.R;
import md.ble.ui.adapters.BleServicesAdapter;
import md.ble.ui.dialogs.EnterValueDialog;

public class DeviceServicesActivity extends Activity
        implements
        BleServicesAdapter.OnServiceItemClickListener,
        EnterValueDialog.ValueEnterDialogListener,
        SeekBar.OnSeekBarChangeListener,
        BleManagerService.BleStateListener {
    /**
     * Log tag.
     */
    @SuppressWarnings("UnusedDeclaration")
    private final static String TAG = DeviceServicesActivity.class.getSimpleName();


    /**
     * Services list.
     */
    private ExpandableListView gattServicesList;
    /**
     * Services adapter.
     */
    private BleServicesAdapter gattServiceAdapter;


    private InfoService<?> activeSensor = null;
    private BluetoothGattCharacteristic characteristic = null;
    private String address = "";


    List<Entry> entries = new ArrayList<>();
    int entryNo = 1;
    int countToRefreshChart = 0;

    Long lastChanged_PWMDutyCycle = 0L;
    int progress_PWMDutyCycle = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.device_services_activity);
        BleManagerService.getInstance().setBleStateListener(this);
        /*final ActionBar actionBar = getSupportActionBar();
        final String deviceName = getDeviceName();
        if (TextUtils.isEmpty(deviceName)) {
            //noinspection ConstantConditions
            actionBar.setTitle(getDeviceAddress());
        } else {
            //noinspection ConstantConditions
            actionBar.setTitle(deviceName);
            actionBar.setSubtitle(getDeviceAddress());
        }
        actionBar.setDisplayHomeAsUpEnabled(true);*/

        final SeekBar seekBar = (SeekBar) findViewById(R.id.seekBar_pwm);
        seekBar.setOnSeekBarChangeListener(this);

        final SeekBar seekBar2 = (SeekBar) findViewById(R.id.seekBar_vthr);
        seekBar2.setOnSeekBarChangeListener(this);

        final LineChart chart = (LineChart) findViewById(R.id.chart);
        entries.add(new Entry(entryNo++, 0));
        LineDataSet dataSet = new LineDataSet(entries, "Label"); // add entries to dataset
        dataSet.setColor(R.color.text);
        dataSet.setValueTextColor(R.color.text);
        dataSet.setDrawValues(false);

        LineData lineData = new LineData(dataSet);
        chart.setData(lineData);
        chart.invalidate(); // refresh
    }

    @Override
    protected void onDestroy() {
        BleManagerService.getInstance().setBleStateListener(null);
        super.onDestroy();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /*
        @Override
        public void onDisconnected(final String name, final String address) {
            super.onDisconnected(name, address);
            finish();
        }

        @Override
        public void onServiceDiscovered(final String name, final String address) {
            super.onServiceDiscovered(name, address);
        }

        @Override
        public void onCharacteristicChanged(final String name, final String address,
                                            final String serviceUuid,
                                            final String characteristicUuid) {
            super.onCharacteristicChanged(name, address, serviceUuid, characteristicUuid);
            final Sensor<?> sensor = (Sensor<?>) getBleManager().getDeviceDefCollection()
                    .get(name, address).getSensor(serviceUuid);
            if (sensor == null)
                return;
            String dataString = "";
            if (sensor instanceof InfoService) {
                final InfoService<?> infoSensor = (InfoService<?>) sensor;
                dataString = infoSensor.getValue();
            } else {
                final Object data = sensor.getData();
                dataString = String.valueOf(data);
            }
            switch (serviceUuid) {
                case MyCustomService.UUID_SERVICE:
                    switch (characteristicUuid) {
                        case MyCustomService.UUID_PWM_DUTY_CYCLE_ID:

                            final SeekBar et = (SeekBar) findViewById(R.id.seekBar_pwm);
                            et.setVerticalScrollbarPosition(Integer.parseInt(dataString));
                            break;
                        case MyCustomService.UUID_BIKE_BATTERY_LEVEL_ID:

                            final LineChart chart = (LineChart) findViewById(R.id.chart);
                            chart.getLineData().addEntry(new Entry(entryNo++, ((MyCustomService) sensor).getIntValue()), 0);
                            if (countToRefreshChart++ > 10) {
                                chart.notifyDataSetChanged();
                                //chart.getLineData().addEntry(new Entry(entryNo++, ((MyCustomService)sensor).getIntValue()), 0);
                                chart.invalidate(); // refresh
                                countToRefreshChart = 0;
                            }
                            break;
                    }
                    break;
            }

        }
    */
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        SendData(seekBar, false);
    }

    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    public void onStopTrackingTouch(SeekBar seekBar) {
        SendData(seekBar, true);
    }

    @Override
    public void onServiceUpdated(BluetoothGattService service) {

    }


    public void ValueEntered(String value) {
        if (gattServiceAdapter == null)
            return;
        Bundle bundle = new Bundle();
        bundle.putString(BLEConst.DATA, value);

        BleManagerService.getInstance().update((MyCustomService) activeSensor, characteristic.getUuid().toString(), bundle);
        // getBleManager().read(address, sensor, characteristic.getUuid().toString());
    }


    private void SendData(SeekBar seekBar, boolean ignoreTimestamp) {

        final int progress = seekBar.getProgress();
        if (progress_PWMDutyCycle == progress)
            return;

        if (!ignoreTimestamp) {
            Long tsLong = System.currentTimeMillis();
            if (tsLong - lastChanged_PWMDutyCycle < 100)
                return;
            lastChanged_PWMDutyCycle = tsLong;
            progress_PWMDutyCycle = progress;
        }
        final DeviceDef def = BleManagerService.getInstance().getDeviceDef();
        final InfoService<?> sensor = (InfoService<?>) def.getSensor(MyCustomService.UUID_SERVICE);
        Bundle bundle = new Bundle();
        bundle.putString(BLEConst.DATA, Integer.toString(progress));

        switch (seekBar.getId()) {
            case R.id.seekBar_pwm:
                final TextView tvPwm = (TextView) findViewById(R.id.textView_pwmdutycycle);
                tvPwm.setText("PWM Duty Cycle = " + progress);
                BleManagerService.getInstance().update((MyCustomService) sensor, MyCustomService.UUID_PWM_DUTY_CYCLE_ID, bundle);
                break;
            case R.id.seekBar_vthr:
                final TextView tvVthr = (TextView) findViewById(R.id.textView_vthr);
                tvVthr.setText("V Threshold = " + progress);

                BleManagerService.getInstance().update((MyCustomService) sensor, MyCustomService.UUID_V_THRESHOLD_ID, bundle);
                break;
        }
    }

    @Override
    public void onBleStateChange(BleManagerService.bleState state) {
        final BleManagerService.bleState s = state;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                final TextView textview = ((TextView) findViewById(R.id.data_characteristic_uuid));
                switch (s) {
                    case connected:
                        textview.setText("Connected");
                        break;
                    case disconnected:
                        textview.setText("Disconnected");
                        break;
                    case connectionFailed:
                        textview.setText("Connection Failed");
                        break;
                }
                textview.refreshDrawableState();
            }
        });
    }
}
