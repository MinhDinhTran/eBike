package md;

import java.util.HashSet;
import java.util.Set;

public class AppConfig {
    public static final String DEF_EBIKE_CENTRAL = "eBike MC";
    public static final String DEF_EBIKE_PEDAL_R = "eBike Pedal R";
    public static final String DEF_EBIKE_PEDAL_L = "eBike Pedal L";
    public static final String DEF_MI_BAND2_DEVICE_NAME = "MI Band 2";
    public static final Set<String> SUPPORTED_DEVICES = new HashSet<>();
    {
        SUPPORTED_DEVICES.add(DEF_EBIKE_CENTRAL);
        SUPPORTED_DEVICES.add(DEF_EBIKE_PEDAL_R);
        SUPPORTED_DEVICES.add(DEF_EBIKE_PEDAL_L);
        SUPPORTED_DEVICES.add(DEF_MI_BAND2_DEVICE_NAME);
    }
}
