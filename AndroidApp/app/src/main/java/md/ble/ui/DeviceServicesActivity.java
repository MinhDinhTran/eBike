package md.ble.ui;

import android.app.Activity;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ToggleButton;

import md.App;
import md.ble.BLE_Services.BLEConst;
import md.ble.BLE_Services.InfoService;
import md.ble.BLE_Services.MiBand2.HeartRateService;
import md.ble.BLE_Services.MyCustomService;
import md.ble.BLE_Services.MyPedalService;
import md.ble.BleManagerService;
import md.apk.R;
import md.ble.ui.adapters.BleServicesAdapter;

public class DeviceServicesActivity extends Activity
        implements
        BleServicesAdapter.OnServiceItemClickListener,
        SeekBar.OnSeekBarChangeListener,
        BleManagerService.BleStateListener,
        Button.OnClickListener,
        MyCustomService.MyCustomServiceListener,
        HeartRateService.HeartRateServiceListener ,
        MyPedalService.MyPedalServiceListener{

    protected PowerManager.WakeLock mWakeLock;
    @SuppressWarnings("UnusedDeclaration")
    private final static String TAG = DeviceServicesActivity.class.getSimpleName();

    Long lastChanged_PWMDutyCycle = 0L;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.device_services_activity);
        BleManagerService.getInstance().setBleStateListener(this);

        final SeekBar seekBar = (SeekBar) findViewById(R.id.seekBar_pwm);
        seekBar.setOnSeekBarChangeListener(this);
        final SeekBar seekBar2 = (SeekBar) findViewById(R.id.seekBar_vthr);
        seekBar2.setOnSeekBarChangeListener(this);

        final Button button_pwm_down = (Button) findViewById(R.id.button_pwm_down);
        button_pwm_down.setOnClickListener(this);
        final Button button_pwm_up = (Button) findViewById(R.id.button_pwm_up);
        button_pwm_up.setOnClickListener(this);
        final Button button_vthr_down = (Button) findViewById(R.id.button_vthr_down);
        button_vthr_down.setOnClickListener(this);
        final Button button_vthr_up = (Button) findViewById(R.id.button_vthr_up);
        button_vthr_up.setOnClickListener(this);
        final ToggleButton button_Start = (ToggleButton) findViewById(R.id.toggleButton_start);
        button_Start.setOnClickListener(this);
        final Button button_measureHR = (Button) findViewById(R.id.button_MeasureHeartRate);
        button_measureHR.setOnClickListener(this);

        SubscribeBLEServices();
        final PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
        this.mWakeLock.acquire();
    }

    @Override
    protected void onDestroy() {
        BleManagerService.getInstance().setBleStateListener(null);
        this.mWakeLock.release();
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

    private void SendData(SeekBar seekBar, boolean ignoreTimestamp) {

        final int progress = seekBar.getProgress();

        if (!ignoreTimestamp) {
            Long tsLong = System.currentTimeMillis();
            if (tsLong - lastChanged_PWMDutyCycle < 200)
                return;
            lastChanged_PWMDutyCycle = tsLong;
        }
        final InfoService<?> sensor = (InfoService<?>) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
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

    @Override
    public void onClick(View v) {
        final SeekBar seekBar_pwm = (SeekBar) findViewById(R.id.seekBar_pwm);
        final SeekBar seekBar_vthr = (SeekBar) findViewById(R.id.seekBar_vthr);

        switch (v.getId()) {
            case R.id.button_pwm_down:
                seekBar_pwm.setProgress(seekBar_pwm.getProgress() - 3);
                break;
            case R.id.button_pwm_up:
                seekBar_pwm.setProgress(seekBar_pwm.getProgress() + 3);
                break;
            case R.id.button_vthr_down:
                seekBar_vthr.setProgress(seekBar_vthr.getProgress() - 3);
                break;
            case R.id.button_vthr_up:
                seekBar_vthr.setProgress(seekBar_vthr.getProgress() + 3);
                break;
            case R.id.toggleButton_start:

                final ToggleButton button_Start = (ToggleButton) findViewById(R.id.toggleButton_start);
                final InfoService<?> sensor = (InfoService<?>) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
                Bundle bundle = new Bundle();
                bundle.putString(BLEConst.DATA, Integer.toString(button_Start.isChecked() ? 1 : 0));
                BleManagerService.getInstance().update((MyCustomService) sensor, MyCustomService.UUID_MODE_ID, bundle);

                break;
            case R.id.button_MeasureHeartRate:

                final InfoService<?> sensorHR = (InfoService<?>) App.DEVICE_DEF.getSensor(HeartRateService.UUID_SERVICE);
                BleManagerService.getInstance().update((HeartRateService) sensorHR, HeartRateService.UUID_HEARTRATE_CONTROL_ID, null);
                break;
            default:
                break;
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        final SeekBar seekBar_pwm = (SeekBar) findViewById(R.id.seekBar_pwm);
        if (keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
            int index = seekBar_pwm.getProgress();
            seekBar_pwm.setProgress(index + 3);
            return true;
        } else if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
            int index = seekBar_pwm.getProgress();
            seekBar_pwm.setProgress(index - 3);
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }


    private void SubscribeBLEServices() {
        final InfoService<?> sensor = (InfoService<?>) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
        ((MyCustomService) sensor).setServiceListener(this);


        final InfoService<?> sensor2 = (InfoService<?>) App.DEVICE_DEF.getSensor(HeartRateService.UUID_SERVICE);
        ((HeartRateService) sensor2).setServiceListener(this);


        final InfoService<?> sensor3 = (InfoService<?>) App.DEVICE_DEF.getSensor(MyPedalService.UUID_SERVICE);
        ((MyPedalService) sensor3).setServiceListener(this);
    }

    @Override
    public void OnCharacteristicChanged(final String characteristic, final int value) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                TextView textView;
                switch (characteristic) {
                    case MyCustomService.UUID_BIKE_BATTERY_LEVEL_ID:
                        textView = (TextView) findViewById(R.id.textView_bat_variable);
                        textView.setText(Integer.toString(value));
                        textView.refreshDrawableState();
                        break;
                    case MyCustomService.UUID_CURRENT_ID:
                        textView = (TextView) findViewById(R.id.textView_srove_variable);
                        textView.setText(Integer.toString(value));
                        textView.refreshDrawableState();
                        break;
                    case MyCustomService.UUID_BIKE_FLAGS_ID:
                        break;
                    case HeartRateService.UUID_HEARTRATE_MEASURE_ID:
                        textView = (TextView) findViewById(R.id.data_characteristic_value);
                        textView.setText(Integer.toString(value));
                        textView.refreshDrawableState();
                        break;

                    case MyPedalService.UUID_RAW_DATA_ID:
                        textView = (TextView) findViewById(R.id.textView_pedal);
                        textView.setText(Integer.toString(value));
                        textView.refreshDrawableState();
                        break;
                }
            }
        });
    }

    @Override
    public void OnCharacteristicChanged(final String characteristic, final float value) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                TextView textView;
                switch (characteristic) {
                    case MyCustomService.UUID_BIKE_SPEED_ID:
                        textView = (TextView) findViewById(R.id.textView_rpm_variable);
                        textView.setText(Float.toString(value));
                        textView.refreshDrawableState();
                        break;
                }
            }
        });
    }
}
