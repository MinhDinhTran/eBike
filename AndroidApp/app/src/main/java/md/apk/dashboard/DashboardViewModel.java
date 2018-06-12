package md.apk.dashboard;

import android.os.Bundle;

import md.App;
import md.AppConfig;
import md.ble.BLE_Services.BLEConst;
import md.ble.BLE_Services.InfoService;
import md.ble.BLE_Services.MiBand2.HeartRateService;
import md.ble.BLE_Services.MyCustomService;
import md.ble.BLE_Services.MyPedalService;
import md.ble.BleManagerService;

public class DashboardViewModel implements BleManagerService.BleStateListener,
        MyCustomService.Listener,
        HeartRateService.Listener,
        MyPedalService.Listener {
    final static int DUTY_CHANGE_LIMITER = 200; // limits requests to send duty cycle. sends only every 200ms (alto possible to ignore)
    private static DashboardViewModel INSTANCE = null;
    public static DashboardViewModel getInstance() {
        if (INSTANCE == null){
            INSTANCE = new DashboardViewModel();
        }
        return (INSTANCE);
    }

    Long lastChanged_PWMDutyCycle = 0L;

    @Override
    public void onBleStateChange(String name, BleManagerService.bleState state) {
        if (listener!=null)
            listener.onBleStateChange(name, state);
    }


    @Override
    public void OnCharacteristicChanged(final String characteristic, final double value) {
        if (listener == null) return;

        switch (characteristic) {
            case MyCustomService.UUID_BIKE_BATTERY_LEVEL_ID:
                final double bat_V = value * 0.01400529697297297297297297297297;//0.01356534;
                final double bat_level = -0.1416 * bat_V * bat_V * bat_V + 15.745 * bat_V * bat_V - 569.4 * bat_V + 6731.1; // 3.2*((2.2k + 36k)/2.2k) = 55.56 / 4096

                listener.MessageReceived(MSG.BATTERY_VOLTAGE_CHANGED, bat_V);
                listener.MessageReceived(MSG.BATTERY_LEVEL_CHANGED, bat_level);

                break;
            case MyCustomService.UUID_CURRENT_ID:
                final double current = -44.4 + value * 0.0217;//* 0.0217285 - 22.22// (3.2 - 1.6)/(0.001 * 36) = 44.44 / 4096
                listener.MessageReceived(MSG.CURRENT_CHANGED, current);
                break;
            case MyCustomService.UUID_BIKE_FLAGS_ID:
                if (value == 0) {
                    listener.MessageReceived(MSG.OVERCURRENT_CHANGED, 0d);
                }
                else if (value == 1) {
                    listener.MessageReceived(MSG.OVERCURRENT_CHANGED, 1d);
                    listener.MessageReceived(MSG.DUTY_CYCLE_CHANGED, 20d);
                }
                break;
            case HeartRateService.UUID_HEARTRATE_MEASURE_ID:
                listener.MessageReceived(MSG.HEART_RATE_CHANGED, value);
                break;
            case MyCustomService.UUID_PWM_DUTY_CYCLE_ID:
                listener.MessageReceived(MSG.DUTY_CYCLE_CHANGED, value);
                break;
            case MyCustomService.UUID_BIKE_RPM_ID:
                listener.MessageReceived(MSG.BIKE_RPM_CHANGED, value);
                listener.MessageReceived(MSG.BIKE_SPEED_CHANGED, value* 72 * 0.001885);
                break;
            case MyCustomService.UUID_ENERGY_ID:
                listener.MessageReceived(MSG.ENERGY_CHANGED, value);
                break;
        }
    }



    @Override
    public void OnCharacteristicChanged(final String name, final String characteristic,final double value) {
        if (listener == null) return;
        switch (characteristic) {
            case MyPedalService.UUID_RAW_DATA_ID:
                if (name.equals(AppConfig.DEF_EBIKE_PEDAL_L))
                {
                    listener.MessageReceived(MSG.LEFT_PEDAL_FORCE_CHANGED, value);
                } else {
                    listener.MessageReceived(MSG.RIGHT_PEDAL_FORCE_CHANGED, value);
                }
                break;
        }
    }

    public enum MSG{
        DUTY_CYCLE_CHANGED,
        BATTERY_VOLTAGE_CHANGED,
        BATTERY_LEVEL_CHANGED,
        CURRENT_CHANGED,
        OVERCURRENT_CHANGED,
        HEART_RATE_CHANGED,
        BIKE_RPM_CHANGED,
        BIKE_SPEED_CHANGED,
        ENERGY_CHANGED,
        LEFT_PEDAL_FORCE_CHANGED,
        RIGHT_PEDAL_FORCE_CHANGED,
    }
    public interface Listener {
        void MessageReceived(MSG msg, Double value);
        void onBleStateChange(final String name, final BleManagerService.bleState state);
    }
    private Listener listener = null;

    public void setListener(Listener listener){

        this.listener = listener;
        if (listener != null) {
            BleManagerService.getInstance().setBleStateListener(this);
            SubscribeBLEServices(this);
        } else
        {
            BleManagerService.getInstance().setBleStateListener(null);
            SubscribeBLEServices(null);
        }
    }
    private void SubscribeBLEServices(DashboardViewModel listener) {
        final MyCustomService sensor = (MyCustomService) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
        sensor.setServiceListener(this);

        final HeartRateService sensor2 = (HeartRateService) App.DEVICE_DEF.getSensor(HeartRateService.UUID_SERVICE);
        sensor2.setServiceListener(this);

        final MyPedalService sensor3 = (MyPedalService) App.DEVICE_DEF.getSensor(MyPedalService.UUID_SERVICE);
        sensor3.setServiceListener(this);

        final MyPedalService sensor4 = (MyPedalService) App.DEVICE_DEF2.getSensor(MyPedalService.UUID_SERVICE);
        sensor4.setServiceListener(this);
    }
    public void StartMotor(final boolean start){
        final InfoService<?> sensor = (InfoService<?>) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
        Bundle bundle = new Bundle();
        bundle.putString(BLEConst.DATA, Integer.toString(start ? 1 : 0));
        BleManagerService.getInstance().update((MyCustomService) sensor, MyCustomService.UUID_MODE_ID, bundle);

        if(listener != null) {
            if (start) {
                lastChanged_PWMDutyCycle = 0L;
                listener.MessageReceived(MSG.DUTY_CYCLE_CHANGED, 20d);
            } else {
                lastChanged_PWMDutyCycle = 0L;
                listener.MessageReceived(MSG.DUTY_CYCLE_CHANGED, 20d);
            }
        }
    }
    public void SendDutyCycle(final double value, final boolean ignoreTimestamp){
        if (!ignoreTimestamp) {
            Long tsLong = System.currentTimeMillis();
            if (tsLong - lastChanged_PWMDutyCycle < DUTY_CHANGE_LIMITER)
                return;
            lastChanged_PWMDutyCycle = tsLong;
        }
        final InfoService<?> sensor = (InfoService<?>) App.DEVICE_DEF.getSensor(MyCustomService.UUID_SERVICE);
        Bundle bundle = new Bundle();
        bundle.putString(BLEConst.DATA, String.format ("%.0f", value));
        BleManagerService.getInstance().update((MyCustomService) sensor, MyCustomService.UUID_PWM_DUTY_CYCLE_ID, bundle);

        if(listener != null) {
            listener.MessageReceived(MSG.DUTY_CYCLE_CHANGED, value);
        }
    }
}
