package md.apk.dashboard;

import android.app.Activity;
import android.support.v4.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ToggleButton;

import md.AppConfig;
import md.ble.BleManagerService;
import md.apk.R;

public class DashboardFragment extends Fragment
        implements SeekBar.OnSeekBarChangeListener,
        Button.OnClickListener,
        DashboardViewModel.Listener {

    protected PowerManager.WakeLock mWakeLock;
    public final static String TAG = DashboardFragment.class.getSimpleName();
    // Controls
    ToggleButton button_Start = null;
    ToggleButton button_motor = null;
    ToggleButton button_heartrate = null;
    ToggleButton button_leftpedal = null;
    ToggleButton button_right_peda = null;

    /* List<Entry> entries = new ArrayList<Entry>();
     LineDataSet dataSet;
     LineChart chart_itampa;
     LineData lineData;

     long chartDataIndex = 0;
     int chartDatarefreshCounter = 0;*/
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_dashboard, container, false);
        final SeekBar seekBar = (SeekBar) view.findViewById(R.id.seekBar_pwm);
        seekBar.setOnSeekBarChangeListener(this);


        button_Start = (ToggleButton) view.findViewById(R.id.toggleButton_start);
        button_Start.setOnClickListener(this);

        button_motor = (ToggleButton) view.findViewById(R.id.toggleButton_ble_motor);
        button_motor.setOnClickListener(this);
        button_heartrate = (ToggleButton) view.findViewById(R.id.toggleButton_ble_heartrate);
        button_heartrate.setOnClickListener(this);
        button_leftpedal = (ToggleButton) view.findViewById(R.id.toggleButton_ble_leftpedal);
        button_leftpedal.setOnClickListener(this);
        button_right_peda = (ToggleButton) view.findViewById(R.id.toggleButton_ble_right_pedal);
        button_right_peda.setOnClickListener(this);
       /* entries.add(new Entry(0,0));
        dataSet = new LineDataSet(entries, "Itampa");
        lineData = new LineData(dataSet);
        chart_itampa.setData(lineData);

        chart_itampa.invalidate();
        chart_itampa.getXAxis().setGranularity(1f);*/
        return view;
    }

    @Override
    public void onAttach(Context context) {
        DashboardViewModel.getInstance().setListener(this);
        final PowerManager pm = (PowerManager) getActivity().getSystemService(Context.POWER_SERVICE);
        this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
        this.mWakeLock.acquire();

        super.onAttach(context);
    }

    @Override
    public void onDetach() {
        DashboardViewModel.getInstance().setListener(null);
        this.mWakeLock.release();
        super.onDetach();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                getActivity().onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (fromUser && seekBar.getId() == R.id.seekBar_pwm)
            DashboardViewModel.getInstance().SendDutyCycle(seekBar.getProgress(), false);
    }

    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    public void onStopTrackingTouch(SeekBar seekBar) {
        if (seekBar.getId() == R.id.seekBar_pwm)
            DashboardViewModel.getInstance().SendDutyCycle(seekBar.getProgress(), true);
    }


    @Override
    public void MessageReceived(DashboardViewModel.MSG msg, Double value) {
        switch (msg) {
            case DUTY_CYCLE_CHANGED:
                final SeekBar seekBar_pwm = (SeekBar) getActivity().findViewById(R.id.seekBar_pwm);
                seekBar_pwm.setProgress(value.intValue());
                refreshLabelFromWorkerThread(R.id.textView_pwmdutycycle, String.format("PWM Duty Cycle = %.0f", value));
                break;
            case BATTERY_VOLTAGE_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_bat_variable, String.format("%.1f", value));
                break;
            case BATTERY_LEVEL_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_baterijos_lygis, String.format("%.1f", value));
                break;
            case CURRENT_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_srove_variable, String.format("%.1f", value));
                ProgressBar bar = (ProgressBar) getActivity().findViewById(R.id.progressBar3);
                bar.setProgress(value.intValue());
                break;
            case OVERCURRENT_CHANGED:
                final ToggleButton button_Start = (ToggleButton) getActivity().findViewById(R.id.toggleButton_start);
                if (value == 0)
                    button_Start.setBackgroundColor(getResources().getColor(android.R.color.darker_gray));
                else {
                    button_Start.setBackgroundColor(getResources().getColor(android.R.color.holo_red_light));
                    if (button_Start.isChecked())
                        button_Start.toggle();
                }
                break;
            case HEART_RATE_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_heartrate_value, String.format("%.0f", value));
                break;
            case BIKE_RPM_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_rpm_variable, String.format("%.1f", value));
                break;
            case BIKE_SPEED_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_speed, String.format("%.1f", value));

                break;
            case ENERGY_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_galia, String.format("%.1f", value));

                break;
            case LEFT_PEDAL_FORCE_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_leftpedal_value, String.format("%.0f", value));
                ProgressBar barL = (ProgressBar) getActivity().findViewById(R.id.progressBar4);
                barL.setProgress(value.intValue());
                break;
            case RIGHT_PEDAL_FORCE_CHANGED:
                refreshLabelFromWorkerThread(R.id.textView_rightpedal_value, String.format("%.0f", value));
                ProgressBar barR = (ProgressBar) getActivity().findViewById(R.id.progressBar5);
                barR.setProgress(value.intValue());
                break;
        }
    }

    @Override
    public void onBleStateChange(final String name, final BleManagerService.bleState state) {
        final int color = state == BleManagerService.bleState.connected ?
                getResources().getColor(android.R.color.holo_green_dark) : state == BleManagerService.bleState.connectionFailed ?
                getResources().getColor(android.R.color.holo_red_light) :
                getResources().getColor(android.R.color.holo_red_dark);

        getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ToggleButton toggleButton = null;
                switch (name) {
                    case AppConfig.DEF_EBIKE_CENTRAL:
                        toggleButton = (ToggleButton) getActivity().findViewById(R.id.toggleButton_ble_motor);
                        break;
                    case AppConfig.DEF_MI_BAND2_DEVICE_NAME:
                        toggleButton = (ToggleButton) getActivity().findViewById(R.id.toggleButton_ble_heartrate);
                        break;
                    case AppConfig.DEF_EBIKE_PEDAL_L:
                        toggleButton = (ToggleButton) getActivity().findViewById(R.id.toggleButton_ble_leftpedal);
                        break;
                    case AppConfig.DEF_EBIKE_PEDAL_R:
                        toggleButton = (ToggleButton) getActivity().findViewById(R.id.toggleButton_ble_right_pedal);
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

        switch (v.getId()) {
            case R.id.toggleButton_start:
                final ToggleButton button_Start = (ToggleButton) getActivity().findViewById(R.id.toggleButton_start);
                DashboardViewModel.getInstance().StartMotor(button_Start.isChecked());
                break;
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

    private void refreshLabelFromWorkerThread(final int id, final String text) {
        final Activity act = getActivity();
        if (act != null) {
            act.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    final TextView textView = (TextView) act.findViewById(id);
                    if (textView != null) {
                        textView.setText(text);// 3.2*((2.2k + 36k)/2.2k) = 55.56 / 4096
                        textView.refreshDrawableState();
                    }
                }
            });
        }
    }
}
