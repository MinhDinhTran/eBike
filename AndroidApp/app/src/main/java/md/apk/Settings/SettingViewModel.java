package md.apk.Settings;

public class SettingViewModel {
    private static SettingViewModel INSTANCE = null;

    public static SettingViewModel getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new SettingViewModel();
        }
        return (INSTANCE);
    }
}
