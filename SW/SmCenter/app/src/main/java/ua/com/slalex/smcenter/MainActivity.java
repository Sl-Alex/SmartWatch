package ua.com.slalex.smcenter;

import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import ua.com.slalex.smcenter.services.SmWatchService;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener,
        SettingsFragment.OnFragmentInteractionListener,
        AboutFragment.OnFragmentInteractionListener,
        FiltersFragment.OnFragmentInteractionListener,
        LogFragment.OnFragmentInteractionListener,
        MainFragment.OnFragmentInteractionListener,
        ScanFragment.OnFragmentInteractionListener
        {

    //private final static int REQUEST_ENABLE_BT = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        startService(new Intent(this, SmWatchService.class));

        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_main, new MainFragment());
        ft.commit();

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);

        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.setDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
        //BluetoothManager btManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);

        //BluetoothAdapter btAdapter = btManager.getAdapter();
        //if (btAdapter != null && !btAdapter.isEnabled()) {
//            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
//            startActivityForResult(enableIntent,REQUEST_ENABLE_BT);
//        }
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.

        return super.onOptionsItemSelected(item);
    }

    public void onScanButton(View view) {
        FragmentManager fragmentManager = getSupportFragmentManager();
        final FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_main, new ScanFragment());
        ft.commit();
    }



    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        // Create a new fragment and specify the planet to show based on position

        //TextView tv = (TextView) findViewById(R.id.main_text);
        Fragment fragment = null;
        //Bundle args;
        FragmentManager fragmentManager = getSupportFragmentManager();
        switch (id)
        {
            case R.id.nav_about:
                fragment = new AboutFragment();
                break;
            case R.id.nav_settings:
                fragment = new SettingsFragment();
                //args = new Bundle();
                //args.putInt(PlanetFragment.ARG_PLANET_NUMBER, position);
                //fragment.setArguments(args);

                // Insert the fragment by replacing any existing fragment
                //fragmentManager = getSupportFragmentManager();
                //fragmentManager.beginTransaction()
                //        .replace(R.id.content_main, fragment)
                //        .commit();
                break;
            case R.id.nav_filters:
                fragment = new FiltersFragment();
                break;
            case R.id.nav_log:
                fragment = new LogFragment();
                break;
            default:
                fragment = new MainFragment();
        }

        if (fragment != null)
        {
            FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
            ft.replace(R.id.content_main, fragment);
            ft.commit();
        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    public void onSettingsFragmentInteraction(Uri uri) {

    }

    @Override
    public void onAboutFragmentInteraction(Uri uri) {

    }

    @Override
    public void onFiltersFragmentInteraction(Uri uri) {

    }

    @Override
    public void onLogFragmentInteraction(Uri uri) {

    }

    @Override
    public void onMainFragmentInteraction(Uri uri) {

    }


    @Override
    public void onScanFragmentInteraction(Uri uri) {

    }
}
