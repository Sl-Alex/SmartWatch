package ua.com.slalex.smcenter;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.provider.Settings;
import android.support.design.widget.NavigationView;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.support.annotation.NonNull;

import java.util.ArrayList;

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

    private static final int REQUEST_ENABLE_PERMISSIONS = 1;

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
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        checkPermissions();
    }

    /**
     * Check if we have all permissions, request if not all are granted
     */
    private void checkPermissions() {
        ArrayList<String> requiredPerms = new ArrayList<>();
        ArrayList<Integer> checkResults = new ArrayList<>();
        requiredPerms.add(Manifest.permission.ACCESS_COARSE_LOCATION);
        requiredPerms.add(Manifest.permission.RECEIVE_SMS);
        requiredPerms.add(Manifest.permission.READ_CONTACTS);

        int deniedCount = 0;
        for (String required: requiredPerms)
        {
            int result = ContextCompat.checkSelfPermission(this,required);
            checkResults.add(result);
            if (result != PackageManager.PERMISSION_GRANTED)
                deniedCount++;
        }

        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": " + deniedCount + " permission(s) are requested");
        String[] permissions = new String[deniedCount];
        int cnt = 0;
        for (int i = 0; i < requiredPerms.size(); i++)
        {
            if (checkResults.get(i) == PackageManager.PERMISSION_DENIED)
            {
                permissions[cnt] = requiredPerms.get(i);
            }
        }

        // Request for permissions
        if (deniedCount != 0)
            ActivityCompat.requestPermissions(this, permissions, REQUEST_ENABLE_PERMISSIONS);

        if (!Settings.Secure.getString(getContentResolver(),
                "enabled_notification_listeners").contains(getPackageName()))
        {
            String action = "android.settings.ACTION_NOTIFICATION_LISTENER_SETTINGS";
            startActivity(new Intent(action));
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode != REQUEST_ENABLE_PERMISSIONS)
            //noinspection UnnecessaryReturnStatement
            return;
        // TODO or not to do: Here we can parse the result
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
        final FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_main, new ScanFragment());
        ft.commit();
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        // Handle navigation view item clicks here.
        int id = item.getItemId();

        Fragment fragment;
        switch (id)
        {
            case R.id.nav_about:
                fragment = new AboutFragment();
                break;
            case R.id.nav_settings:
                fragment = new SettingsFragment();
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

        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_main, fragment);
        ft.commit();

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
