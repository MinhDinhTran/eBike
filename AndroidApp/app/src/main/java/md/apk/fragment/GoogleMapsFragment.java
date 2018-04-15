package md.apk.fragment;


import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;

public class GoogleMapsFragment extends SupportMapFragment implements OnMapReadyCallback {
    public static final String TAG = "GoogleMapsFragment";

    private GoogleMap mMap;
    private int PERMISSION_REQUEST_CODE = 0;

    public GoogleMapsFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        getMapAsync(this);
        return super.onCreateView(inflater, container, savedInstanceState);
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(getActivity(),
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_CODE);
            return;
        }

        setMyLocation();
    }
        @Override
       public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                              int[] grantResults)
       {
            if (requestCode == PERMISSION_REQUEST_CODE)
                setMyLocation();
       }

       private void setMyLocation() {
           if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
              // ActivityCompat.requestPermissions(getActivity(),
               //        new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_CODE);
               return;
           }
           mMap.setMyLocationEnabled(true);
           LocationManager locationManager = (LocationManager) getContext().getSystemService(Context.LOCATION_SERVICE);
           Criteria criteria = new Criteria();
           Location location = locationManager.getLastKnownLocation(locationManager.getBestProvider(criteria, false));
           if (location != null)
           {
               //mMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location.getLatitude(), location.getLongitude()), 13));

               CameraPosition cameraPosition = new CameraPosition.Builder()
                       .target(new LatLng(location.getLatitude(), location.getLongitude()))      // Sets the center of the map to location user
                       .zoom(17)                   // Sets the zoom
                       .bearing(0)                // Sets the orientation of the camera to north
                       .tilt(0)                   // Sets the tilt of the camera to 0 degrees
                       .build();                   // Creates a CameraPosition from the builder
               mMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
           }
       }
}
