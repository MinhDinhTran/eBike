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
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ToggleButton;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineDataSet;

import java.util.ArrayList;
import java.util.List;

import md.App;
import md.AppConfig;
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
    // Controls
    Button button_pwm_down = null;
    Button button_pwm_up = null;
    Button button_vthr_down = null;
    Button button_vthr_up = null;
    ToggleButton button_Start = null;
    ToggleButton button_motor = null;
    ToggleButton button_heartrate= null;
    ToggleButton button_leftpedal = null;
    ToggleButton button_right_peda = null;
    //
    Long lastChanged_PWMDutyCycle = 0L;
    List<Entry> entries = new ArrayList<Entry>();
    LineDataSet dataSet;
    LineChart chart_itampa;
    long chartDataIndex = 0;
    int chartDatarefreshCounter = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.device_services_activity);
        BleManagerService.getInstance().setBleStateListener(this);

        final SeekBar seekBar = (SeekBar) findViewById(R.id.seekBar_pwm);
        seekBar.setOnSeekBarChangeListener(this);
        final SeekBar seekBar2 = (SeekBar) findViewById(R.id.seekBar_vthr);
        seekBar2.setOnSeekBarChangeListener(this);

        button_pwm_down = (Button) findViewById(R.id.button_pwm_down);
        button_pwm_down.setOnClickListener(this);
        button_pwm_up = (Button) findViewById(R.id.button_pwm_up);
        button_pwm_up.setOnClickListener(this);
        button_vthr_down = (Button) findViewById(R.id.button_vthr_down);
        button_vthr_down.setOnClickListener(this);
        button_vthr_up = (Button) findViewById(R.id.button_vthr_up);
        button_vthr_up.setOnClickListener(this);
        button_Start = (ToggleButton) findViewById(R.id.toggleButton_start);
        button_Start.setOnClickListener(this);

        button_motor = (ToggleButton) findViewById(R.id.toggleButton_ble_motor);
        button_motor.setOnClickListener(this);
        button_heartrate= (ToggleButton) findViewById(R.id.toggleButton_ble_heartrate);
        button_heartrate.setOnClickListener(this);
        button_leftpedal = (ToggleButton) findViewById(R.id.toggleButton_ble_leftpedal);
        button_leftpedal.setOnClickListener(this);
        button_right_peda = (ToggleButton) findViewById(R.id.toggleButton_ble_right_pedal);
        button_right_peda.setOnClickListener(this);







       /* entries.add(new Entry(0,0));
        dataSet = new LineDataSet(entries, "Itampa");
        chart_itampa = (LineChart) findViewById(R.id.chart_itampa);
        chart_itampa.setData( new LineData(dataSet));
        chart_itampa.invalidate(); //
        chart_itampa.setVisibleXRangeMaximum(100);*/

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
                int val = progress*270+3000;
                tvVthr.setText("V Threshold = " + val);

                BleManagerService.getInstance().update((MyCustomService) sensor, MyCustomService.UUID_V_THRESHOLD_ID, bundle);
                break;
        }
    }

    @Override
    public void onBleStateChange(final String name, final BleManagerService.bleState state) {
        final int color = state == BleManagerService.bleState.connected ?
                getResources().getColor(android.R.color.holo_green_dark) : state == BleManagerService.bleState.connectionFailed ?
                getResources().getColor(android.R.color.holo_red_light) :
                getResources().getColor(android.R.color.holo_red_dark);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ToggleButton toggleButton = null;
                switch (name) {
                    case AppConfig.DEF_EBIKE_CENTRAL:
                        toggleButton = (ToggleButton) findViewById(R.id.toggleButton_ble_motor);
                        break;
                    case AppConfig.DEF_MI_BAND2_DEVICE_NAME:
                        toggleButton = (ToggleButton) findViewById(R.id.toggleButton_ble_heartrate);
                        break;
                    case AppConfig.DEF_EBIKE_PEDAL_L:
                        toggleButton = (ToggleButton) findViewById(R.id.toggleButton_ble_leftpedal);
                        break;
                    case AppConfig.DEF_EBIKE_PEDAL_R:
                        toggleButton = (ToggleButton) findViewById(R.id.toggleButton_ble_right_pedal);
                        break;
                }
                if (toggleButton != null) {
                    toggleButton.setBackgroundColor(color);
                    toggleButton.setChecked(state == BleManagerService.bleState.connected);
                    toggleButton.refreshDrawableState();
                }
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
          /*  case R.id.button_MeasureHeartRate:

                final InfoService<?> sensorHR = (InfoService<?>) App.DEVICE_DEF.getSensor(HeartRateService.UUID_SERVICE);
                BleManagerService.getInstance().update((HeartRateService) sensorHR, HeartRateService.UUID_HEARTRATE_CONTROL_ID, null);
                break;*/


            case R.id.toggleButton_ble_motor:
                if (button_motor.isChecked())
                    BleManagerService.getInstance().ConnectToDevice(AppConfig.DEF_EBIKE_CENTRAL);
                else
                    BleManagerService.getInstance().CancelSearch(AppConfig.DEF_EBIKE_CENTRAL);
                break;
            case R.id.toggleButton_ble_heartrate:
                if (button_heartrate.isChecked())
                    BleManagerService.getInstance().ConnectToDevice(AppConfig.DEF_MI_BAND2_DEVICE_NAME);
                else
                    BleManagerService.getInstance().CancelSearch(AppConfig.DEF_MI_BAND2_DEVICE_NAME);
                break;
            case R.id.toggleButton_ble_leftpedal:
                if (button_leftpedal.isChecked())
                    BleManagerService.getInstance().ConnectToDevice(AppConfig.DEF_EBIKE_PEDAL_L);
                else
                    BleManagerService.getInstance().CancelSearch(AppConfig.DEF_EBIKE_PEDAL_L);
                break;
            case R.id.toggleButton_ble_right_pedal:
                if (button_right_peda.isChecked())
                    BleManagerService.getInstance().ConnectToDevice(AppConfig.DEF_EBIKE_PEDAL_R);
                else
                    BleManagerService.getInstance().CancelSearch(AppConfig.DEF_EBIKE_PEDAL_R);
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
        final MyCustomService sensor = (MyCustomService) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
        sensor.setServiceListener(this);

        final HeartRateService sensor2 = (HeartRateService) App.DEVICE_DEF.getSensor(HeartRateService.UUID_SERVICE);
        sensor2.setServiceListener(this);

        final MyPedalService sensor3 = (MyPedalService) App.DEVICE_DEF.getSensor(MyPedalService.UUID_SERVICE);
        sensor3.setServiceListener(this);

        final MyPedalService sensor4 = (MyPedalService) App.DEVICE_DEF2.getSensor(MyPedalService.UUID_SERVICE);
        sensor4.setServiceListener(this);
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
                        textView.setText(String.format ("%.2f", value*0.01356534));// 3.2*((2.2k + 36k)/2.2k) = 55.56 / 4096
                        textView.refreshDrawableState();
                        break;
                    case MyCustomService.UUID_CURRENT_ID:
                        textView = (TextView) findViewById(R.id.textView_srove_variable);
                        textView.setText( String.valueOf(value)); //* 0.0217285 - 22.22// (3.2 - 1.6)/(0.001 * 36) = 44.44 / 4096

                        ProgressBar bar = (ProgressBar) findViewById(R.id.progressBar3);
                        bar.setProgress(value);
                        //  dataSet.addEntry();
                       /* entries.add(new Entry(chartDataIndex++, value));
                        if (chartDataIndex > 500) {
                            dataSet.removeEntry(0);
                        }
                        if (++chartDatarefreshCounter > 50){
                            chartDatarefreshCounter = 0;
                            dataSet.notifyDataSetChanged();
                            chart_itampa.notifyDataSetChanged();
                            chart_itampa.fitScreen();
                           // chart_itampa.invalidate();
                        }*/
                        textView.refreshDrawableState();

                        break;
                    case MyCustomService.UUID_BIKE_FLAGS_ID:
                        final ToggleButton button_Start = (ToggleButton) findViewById(R.id.toggleButton_start);
                        if (value == 0)
                            button_Start.setBackgroundColor(getResources().getColor(android.R.color.darker_gray));
                        else {
                            button_Start.setBackgroundColor(getResources().getColor(android.R.color.holo_red_light));
                            if (button_Start.isChecked())
                                button_Start.toggle();
                            final SeekBar seekBar_pwm1 = (SeekBar) findViewById(R.id.seekBar_pwm);
                            seekBar_pwm1.setProgress(20);
                        }
                        break;
                    case HeartRateService.UUID_HEARTRATE_MEASURE_ID:
                        textView = (TextView) findViewById(R.id.textView_heartrate_value);
                        textView.setText(Integer.toString(value));
                        textView.refreshDrawableState();
                        break;
                    case MyCustomService.UUID_PWM_DUTY_CYCLE_ID:
                        final SeekBar seekBar_pwm = (SeekBar) findViewById(R.id.seekBar_pwm);
                        seekBar_pwm.setProgress(value);
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
                        textView.setText(String.format ("%.1f", value));
                        textView.refreshDrawableState();
                        break;
                }
            }
        });
    }

    @Override
    public void OnCharacteristicChanged(final String name, final String characteristic,final int value) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {

                TextView textView;
                ProgressBar bar;
                switch (characteristic) {
                    case MyPedalService.UUID_RAW_DATA_ID:
                        if (name.equals(AppConfig.DEF_EBIKE_PEDAL_L)){
                            textView = (TextView) findViewById(R.id.textView_leftpedal_value);
                            bar = (ProgressBar) findViewById(R.id.progressBar4);
                        }
                        else {
                            bar = (ProgressBar) findViewById(R.id.progressBar5);
                            textView = (TextView) findViewById(R.id.textView_rightpedal_value);
                        }
                        textView.setText(Integer.toString(value));
                        bar.setProgress(value);
                        textView.refreshDrawableState();
                        break;
                }
            }
        });
    }
}
