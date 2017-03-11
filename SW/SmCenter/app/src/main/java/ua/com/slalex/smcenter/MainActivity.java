package ua.com.slalex.smcenter;

import android.Manifest;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
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
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;

import ua.com.slalex.smcenter.BLE.BleTransferTask;
import ua.com.slalex.smcenter.services.SmWatchService;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener,
        SettingsFragment.OnFragmentInteractionListener,
        FiltersFragment.OnFragmentInteractionListener,
        LogFragment.OnFragmentInteractionListener,
        MainFragment.OnFragmentInteractionListener,
        ScanFragment.OnFragmentInteractionListener,
        SmWatchService.SmWatchServiceIface
{

    private static final int REQUEST_ENABLE_PERMISSIONS = 1;
    private Intent mServiceIntent;
    private ServiceConnection mServiceConnection;
    private boolean mServiceConnected = false;
    private SmWatchService mService = null;
    private MainActivity mThis;

    private Fragment mSelectedFragment = null;
    private int mSelectedFragmentId = -1;
    SimpleDateFormat mDateTimeFormat = new SimpleDateFormat("yyyy-MM-dd; k:mm");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mThis = this;

        mServiceIntent = new Intent(this, SmWatchService.class);
        startService(mServiceIntent);

        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_main, new MainFragment());
        ft.commit();
        mSelectedFragmentId = R.id.nav_main;

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);

        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        checkPermissions();

        mServiceConnection = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
                mServiceConnected = true;
                Log.d(Constants.LOG_TAG, mThis.getClass().getSimpleName() + ": Service is connected");
                mService = ((SmWatchService.SmWatchBinder)iBinder).getService();
                mService.setCallbackIface(mThis);
            }

            @Override
            public void onServiceDisconnected(ComponentName componentName) {
                mServiceConnected = false;
                Log.d(Constants.LOG_TAG, mThis.getClass().getSimpleName() + ": Service is disconnected");
                mService = null;
            }
        };
        bindService(mServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
    }

    public ArrayList<BleTransferTask> getLastResults()
    {
        if (mService == null)
            return null;

        return mService.getLastResults();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mServiceConnected) {
            unbindService(mServiceConnection);
            mServiceConnected = false;
        }
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

        switch (id)
        {
            case R.id.nav_about:
                mSelectedFragment = new AboutFragment();
                mSelectedFragmentId = id;
                break;
            case R.id.nav_settings:
                mSelectedFragment = new SettingsFragment();
                mSelectedFragmentId = id;
                break;
            case R.id.nav_filters:
                mSelectedFragment = new FiltersFragment();
                mSelectedFragmentId = id;
                break;
            case R.id.nav_log:
                mSelectedFragment = new LogFragment();
                mSelectedFragmentId = id;
                break;
            case R.id.nav_main:
                mSelectedFragment = new MainFragment();
                mSelectedFragmentId = id;
        }

        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        ft.replace(R.id.content_main, mSelectedFragment);
        ft.commit();

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    public void onSettingsFragmentInteraction(Uri uri) {

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

    @Override
    public void onLogUpdated(final ArrayList<BleTransferTask> results) {
        Log.d(Constants.LOG_TAG, this.getClass().getSimpleName() + ": onLogUpdated");
        if (mSelectedFragmentId == R.id.nav_log) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ArrayList<BleTransferTask> loc_results = results;
                    if (loc_results == null)
                    {
                        loc_results = mThis.getLastResults();
                    }
                    ListView lv = (ListView) findViewById(R.id.logListView);
                    if (lv == null)
                        return;

                    HashMap<String, String> map = new HashMap<>();
                    ArrayList<HashMap<String, String>> myArrList = new ArrayList<>();
                    for (int i = loc_results.size() - 1; i >= 0; i--) {
                        BleTransferTask result = loc_results.get(i);
                        map.put("Header", result.toString(result.type) + " (" + mDateTimeFormat.format(result.datetime.getTime()) + ")");
                        if (result.status) {
                            map.put("Text", "succeeded, RSSI = " + result.rssi);
                        }
                        else {
                            map.put("Text", "failed");
                        }
                        myArrList.add(map);
                    }
                    SimpleAdapter adapter = new SimpleAdapter(mThis, myArrList, android.R.layout.simple_list_item_2,
                            new String[] {"Header", "Text"},
                            new int[] {android.R.id.text1, android.R.id.text2});
                    lv.setAdapter(adapter);
                }
            });
        } else if (mSelectedFragmentId == R.id.nav_main) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ArrayList<BleTransferTask> loc_results = results;
                    if (loc_results == null)
                    {
                        loc_results = mThis.getLastResults();
                    }
                    if ((loc_results == null) || (loc_results.size() == 0))
                        return;
                    TextView tv = (TextView) findViewById(R.id.lastConnectedText);
                    if (tv == null)
                        return;
                    if (!loc_results.get(loc_results.size() - 1).status)
                        tv.setText(mDateTimeFormat.format(loc_results.get(loc_results.size() - 1).datetime.getTime()) + " (failed)");
                    else
                        tv.setText(mDateTimeFormat.format(loc_results.get(loc_results.size() - 1).datetime.getTime()));
                    tv = (TextView) findViewById(R.id.lastRssiText);
                    if (tv == null)
                        return;
                    tv.setText(Integer.toString(loc_results.get(loc_results.size() - 1).rssi));
                }
            });
        }
    }
}
