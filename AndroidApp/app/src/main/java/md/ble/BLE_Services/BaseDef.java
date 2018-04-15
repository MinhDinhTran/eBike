package md.ble.BLE_Services;

import com.chimeraiot.android.ble.sensor.DeviceDef;
import com.chimeraiot.android.ble.sensor.Sensor;

import java.util.ArrayList;
import java.util.List;

/** BLE base sensor group. */
public class BaseDef<M> extends DeviceDef<M> {
    /** Collection of sensors. */
    private final List<Sensor<M>> sensors = new ArrayList<>();

    /**
     * Constructor.
     * @param model - sensor model.
     */
    public BaseDef(M model) {
        super(model);
        sensors.add(new GapService<M>(model));
        sensors.add(new DeviceInfoService<M>(model));
        sensors.add(new GattService<M>(model));
        sensors.add(new MyCustomService<M>(model));
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
