package md;

import java.util.HashSet;
import java.util.Set;

public class AppConfig {
    public static final String DEF_DEVICE_NAME = "eBike MC";
    public static final String DEF_MI_BAND2_DEVICE_NAME = "MI Band 2";
    public static final Set<String> SUPPORTED_DEVICES = new HashSet<>();
    {
        SUPPORTED_DEVICES.add(DEF_DEVICE_NAME);
        SUPPORTED_DEVICES.add(DEF_MI_BAND2_DEVICE_NAME);
    }
}
