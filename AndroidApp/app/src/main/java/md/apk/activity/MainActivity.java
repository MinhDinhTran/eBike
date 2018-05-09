package md.apk.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import com.bumptech.glide.Glide;
import com.bumptech.glide.load.engine.DiskCacheStrategy;

import md.App;
import md.DB.SensorDataController;
import md.ble.ui.DeviceServicesActivity;
import md.apk.R;
import md.apk.fragment.ConnectedDeviceFragment;
import md.apk.fragment.GoogleMapsFragment;
import md.apk.fragment.HistoryFragment;
import md.apk.fragment.HomeFragment;
import md.apk.fragment.ProfileFragment;
import md.apk.fragment.SettingsFragment;
import md.apk.other.Chronometer;

public class MainActivity extends AppCompatActivity {
    private NavigationView navigationView;
    private DrawerLayout drawer;
    private View navHeader;
    private ImageView imgNavHeaderBg;
    private Toolbar toolbar;
    private FloatingActionButton fab;

    private boolean isChronometerStarted = false;
    public static int navItemIndex = 0;
    public static String CURRENT_TAG = HomeFragment.TAG;
    private String[] activityTitles;
    private boolean shouldLoadHomeFragOnBackPress = true;
    private Handler mHandler;
    private SensorDataController _sensorDataController = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        mHandler = new Handler();

        drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        navigationView = (NavigationView) findViewById(R.id.nav_view);
        fab = (FloatingActionButton) findViewById(R.id.fab);

        // Navigation view header
        navHeader = navigationView.getHeaderView(0);
        imgNavHeaderBg = (ImageView) navHeader.findViewById(R.id.img_header_bg);

        // load toolbar titles from string resources
        activityTitles = getResources().getStringArray(R.array.nav_item_activity_titles);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (navItemIndex == 0) {
                    navItemIndex = 1;
                    CURRENT_TAG = GoogleMapsFragment.TAG;
                } else {
                    navItemIndex = 0;
                    CURRENT_TAG = HomeFragment.TAG;
                }
                loadHomeFragment();
            }
        });

        loadNavHeader();
        setUpNavigationView();

        if (savedInstanceState == null) {
            navItemIndex = 0;
            CURRENT_TAG = HomeFragment.TAG;
            loadHomeFragment();
        }


        //BleSensorController a  = new BleSensorController(this);
        _sensorDataController = new SensorDataController(this, null);
        App.sensorDataController = _sensorDataController;

        startActivity(new Intent(MainActivity.this, DeviceServicesActivity.class));
        drawer.closeDrawers();
    }

    private void loadNavHeader() {
        Glide.with(this).load(R.drawable.nav_menu_header_bg)
                .crossFade()
                .diskCacheStrategy(DiskCacheStrategy.ALL)
                .into(imgNavHeaderBg);
    }

    private void loadHomeFragment() {
        selectNavMenu();
        setToolbarTitle();
        if (getSupportFragmentManager().findFragmentByTag(CURRENT_TAG) != null) {
            drawer.closeDrawers();
            toggleFab();
            return;
        }

        Runnable mPendingRunnable = new Runnable() {
            @Override
            public void run() {
                Fragment fragment = getHomeFragment();
                FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
                fragmentTransaction.setCustomAnimations(android.R.anim.fade_in,
                        android.R.anim.fade_out);
                fragmentTransaction.replace(R.id.frame, fragment, CURRENT_TAG);
                fragmentTransaction.commit();
            }
        };

        if (mPendingRunnable != null) {
            mHandler.post(mPendingRunnable);
        }

        toggleFab();
        drawer.closeDrawers();
        invalidateOptionsMenu();
    }
    private Fragment getHomeFragment() {
        switch (navItemIndex) {
            case 0:
                return new HomeFragment();
            case 1:
                return new GoogleMapsFragment();
            case 2:
                return new HistoryFragment();
            case 3:
                return new ProfileFragment();
            case 4:
                return new ConnectedDeviceFragment();
            case 5:
                return new SettingsFragment();
            default:
                return new HomeFragment();
        }
    }

    private void setToolbarTitle() {
        getSupportActionBar().setTitle(activityTitles[navItemIndex]);
    }

    private void selectNavMenu() {
        navigationView.getMenu().getItem(navItemIndex).setChecked(true);
    }

    private void setUpNavigationView() {
        navigationView.setNavigationItemSelectedListener(new NavigationView.OnNavigationItemSelectedListener() {

            // This method will trigger on item Click of navigation menu
            @Override
            public boolean onNavigationItemSelected(MenuItem menuItem) {
                switch (menuItem.getItemId()) {
                    case R.id.nav_home:
                        navItemIndex = 0;
                        CURRENT_TAG = HomeFragment.TAG;
                        break;
                    case R.id.nav_maps:
                        navItemIndex = 1;
                        CURRENT_TAG = GoogleMapsFragment.TAG;
                        break;
                    case R.id.nav_history:
                        navItemIndex = 2;
                        CURRENT_TAG = HistoryFragment.TAG;
                        break;
                    case R.id.nav_profile:
                        navItemIndex = 3;
                        CURRENT_TAG = ProfileFragment.TAG;
                        break;
                    case R.id.nav_connecteddevice:
                        navItemIndex = 4;
                        CURRENT_TAG = ConnectedDeviceFragment.TAG;
                        break;
                    case R.id.nav_settings:
                        navItemIndex = 5;
                        CURRENT_TAG = SettingsFragment.TAG;
                        break;
                    case R.id.nav_bluetooth:
                       // startActivityForResult(new Intent(MainActivity.this, DeviceScanActivity.class), 0);
                        startActivity(new Intent(MainActivity.this, DeviceServicesActivity.class));
                        drawer.closeDrawers();
                        return true;
                    case R.id.nav_about_us:
                        startActivity(new Intent(MainActivity.this, AboutUsActivity.class));
                        drawer.closeDrawers();
                        return true;
                    case R.id.nav_privacy_policy:
                        startActivity(new Intent(MainActivity.this, PrivacyPolicyActivity.class));
                        drawer.closeDrawers();
                        return true;
                    default:
                        navItemIndex = 0;
                }

                if (menuItem.isChecked()) {
                    menuItem.setChecked(false);
                } else {
                    menuItem.setChecked(true);
                }
                menuItem.setChecked(true);
                loadHomeFragment();
                return true;
            }
        });


        ActionBarDrawerToggle actionBarDrawerToggle = new ActionBarDrawerToggle(this, drawer, toolbar, R.string.openDrawer, R.string.closeDrawer) {

            @Override
            public void onDrawerClosed(View drawerView) {
                super.onDrawerClosed(drawerView);
            }

            @Override
            public void onDrawerOpened(View drawerView) {
                super.onDrawerOpened(drawerView);
            }
        };
        drawer.setDrawerListener(actionBarDrawerToggle);
        actionBarDrawerToggle.syncState();
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to

        if (requestCode == 0) {
            // Make sure the request was successful
            if (resultCode == RESULT_OK) {

                final Intent intent = new Intent(this, DeviceServicesActivity.class);
               // intent.putExtra(DeviceServicesActivity.EXTRAS_DEVICE_NAME, data.getStringExtra(DeviceServicesActivity.EXTRAS_DEVICE_NAME));
               // intent.putExtra(DeviceServicesActivity.EXTRAS_DEVICE_ADDRESS, data.getStringExtra(DeviceServicesActivity.EXTRAS_DEVICE_ADDRESS));

                startActivity(intent);
            }
        }
    }
    @Override
    public void onBackPressed() {
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawers();
            return;
        }

        // This code loads home fragment when back key is pressed
        // when user is in other fragment than home
        if (shouldLoadHomeFragOnBackPress) {
            // checking if user is on other navigation menu
            // rather than home
            if (navItemIndex != 0) {
                navItemIndex = 0;
                CURRENT_TAG = HomeFragment.TAG;
                loadHomeFragment();
                return;
            }
        }

        super.onBackPressed();
    }

    private void toggleFab() {
        if (navItemIndex < 2)
            fab.show();
        else
            fab.hide();
    }


    public void StartChronometer(View view) {

        Chronometer mChronometer = (Chronometer) findViewById(R.id.chronometer);
        Button mButton = (Button) findViewById(R.id.button_chronometer);
        if (isChronometerStarted) {
            mChronometer.stop();

            mButton.setText("Start");
            mButton.setBackground(ContextCompat.getDrawable(this, (R.drawable.button_oval_green)));
        }
        else {
            mChronometer.setBase(SystemClock.elapsedRealtime());
            mChronometer.start();
            mButton.setText("Stop");
            mButton.setBackground(ContextCompat.getDrawable(this, (R.drawable.button_oval_yellow)));
        }

        isChronometerStarted = !isChronometerStarted;
    }
}
