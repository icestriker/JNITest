package com.example.admin.jnitest;

import android.Manifest;
import android.accounts.Account;
import android.accounts.AccountManager;
import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.ButtonBarLayout;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity {
    private static IServiceInterface IService;//Binder
    private String TAG = "MainActivity";
    private  Context mContext =MainActivity.this;
    boolean mBound = false;

    public  static String getDeviceID() throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
            Context context=null;
            Class tel_clazz=Class.forName("android.app.ActivityThread");
            context= (Context) tel_clazz.getMethod("currentApplication").invoke(null);
            TelephonyManager tel=(TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
            return tel.getDeviceId();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        new Thread(new ServerThread()).start();
        Button button = (Button) findViewById(R.id.button);
        Button button2 = (Button) findViewById(R.id.button2);
        Button button3 = (Button) findViewById(R.id.button3);
        Button button4=(Button) findViewById(R.id.button4);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    int res = IService.prepare();
                    if (res == 0)
                        Toast.makeText(MainActivity.this, "env指针修改成功", Toast.LENGTH_SHORT).show();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });

        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    String s = IService.getIMEI();

                    Log.d(TAG, "IMEI:" + s);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
              /*  try {
                    Log.d(TAG,"IMEI:"+getDeviceID());
                } catch (ClassNotFoundException e) {
                    e.printStackTrace();
                } catch (NoSuchMethodException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                }*/
            }
        });

        button4.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                try {
                    String signature=IService.getPackageSignature();
                    Log.d(TAG, "onClick: "+signature);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
        button3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                {
                    try {
                        Location location=IService.getLocation(LocationManager.GPS_PROVIDER);
                        if(location!=null)
                            Log.d(TAG, "onClick: "+location.getLongitude());
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
/*                    LocationManager locationManager = (LocationManager)
                            mContext.getSystemService(Context.LOCATION_SERVICE);
                    String provider = "";
                    Location location;
                    List<String> providerList = locationManager.getProviders(true);
                    Log.d("----->providerList: ", providerList.toString());

                    if (providerList.contains(LocationManager.NETWORK_PROVIDER)) {
                        provider = LocationManager.NETWORK_PROVIDER;
                    } else if (providerList.contains(LocationManager.GPS_PROVIDER)) {
                        provider = LocationManager.GPS_PROVIDER;
                    } else {
                        Log.d("错误：", "no provider");
                    }
                    if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                        // TODO: Consider calling
                        return;
                    }
//                    LocationListener mLocationListener=new LocationListener() {
//                        @Override
//                        public void onLocationChanged(Location location) {
//
//                        }
//
//                        @Override
//                        public void onStatusChanged(String provider, int status, Bundle extras) {
//
//                        }
//
//                        @Override
//                        public void onProviderEnabled(String provider) {
//
//                        }
//
//                        @Override
//                        public void onProviderDisabled(String provider) {
//
//                        }
//                    };
//                    locationManager.requestLocationUpdates(locationManager.GPS_PROVIDER,2000,1,mLocationListener);
               *//*     try {
                        location=IService.getLocation(LocationManager.GPS_PROVIDER);
                        if(location!=null)
                            Log.d(TAG, "onClick: "+location.getLongitude());
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }*//*
                    location = locationManager.getLastKnownLocation(provider);
                    if(location!=null)
                    Log.d(TAG, "onClick: location"+location.getLongitude());
                    //        locationManager.requestLocationUpdates(provider, 3000, 1, locationListener);

                }*/

            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
        Intent intent = new Intent(this, IsolatedProcessService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className,
                                       IBinder service) {
            IService = IServiceInterface.Stub.asInterface(service);
            try {

                //注册监听器
                IService.registerOnNewRequestArrivedListener(listener);

                Log.i("listener", "manager.registerOnNewRequestArrivedListener(listener)");
            } catch (RemoteException e) {
                e.printStackTrace();
            }
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            mBound = false;
        }
    };

    IOnNewInvokeRequestListener.Stub listener = new IOnNewInvokeRequestListener.Stub() {
        @Override
        public Location onLocationRequest(String provider) throws RemoteException {
            LocationManager locationManager = (LocationManager)
                    mContext.getSystemService(Context.LOCATION_SERVICE);
            if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(mContext, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

                return null;
            }
            return locationManager.getLastKnownLocation(provider);
        }

        @Override
        public Account[] onAccountRequest() throws RemoteException {
            AccountManager mAccountManager = (AccountManager) mContext.getSystemService(Context.ACCOUNT_SERVICE);
            Account[] accounts = mAccountManager.getAccounts();
           return  accounts;
        }

        @Override
        public String onNewRequest(String method) throws RemoteException {
            Log.d(TAG, "onNewRequest: "+method);
            String[] call=method.split(",");
            String ClassName=call[0];
            String MethodName=call[1];


            if(ClassName.equals("TelephonyManager")&&MethodName.equals("getDeviceId")){
                TelephonyManager telephonyManager= (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
                String IMEI=telephonyManager.getDeviceId();
                return IMEI;
            }

            return "ERROR";
        }
    };

    protected void onStop() {
        super.onStop();
        // Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
    }

}
