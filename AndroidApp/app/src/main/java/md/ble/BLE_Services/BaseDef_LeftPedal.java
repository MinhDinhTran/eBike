package md.ble.BLE_Services;

import com.chimeraiot.android.ble.sensor.DeviceDef;
import com.chimeraiot.android.ble.sensor.Sensor;

import java.util.ArrayList;
import java.util.List;

import md.ble.BLE_Services.MiBand2.AlertService;
import md.ble.BLE_Services.MiBand2.BasicService;
import md.ble.BLE_Services.MiBand2.HeartRateService;

/** BLE base sensor group. */
public class BaseDef_LeftPedal<M> extends DeviceDef<M> {
    /** Collection of sensors. */
    private final List<Sensor<M>> sensors = new ArrayList<>();

    /**
     * Constructor.
     * @param model - sensor model.
     */
    public BaseDef_LeftPedal(M model) {
        super(model);
        sensors.add(new MyPedalService<M>(model));

    }

    @Override
    public Sensor<M> getSensor(String uuid) {
        for (Sensor<M> sensor : sensors) {
            if (sensor.getServiceUUID().equals(uuid)) {
                return sensor;
            }
        }
        return null;
    }

}
