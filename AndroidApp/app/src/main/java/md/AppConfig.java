package md;

import java.util.HashSet;
import java.util.Set;

public class AppConfig {
    public static final String DEF_DEVICE_NAME = "BLEPeripheral";
    public static final Set<String> SUPPORTED_DEVICES = new HashSet<>();
    {
        SUPPORTED_DEVICES.add(DEF_DEVICE_NAME);
    }
}
