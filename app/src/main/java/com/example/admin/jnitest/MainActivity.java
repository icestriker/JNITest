package com.example.admin.jnitest;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
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


public class MainActivity extends AppCompatActivity {
    private static IServiceInterface IService;//Binder
    private String TAG="MainActivity";
    private Context mContext=MainActivity.this;
    boolean mBound = false;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        new Thread(new ServerThread()).start();
        Button button = (Button) findViewById(R.id.button);
        Button button2 = (Button) findViewById(R.id.button2);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    int res=IService.prepare();
                    if(res==0)
                        Toast.makeText(MainActivity.this,"env指针修改成功",Toast.LENGTH_SHORT).show();
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
                    Log.d(TAG, "IMEI:"+s);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }

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
        public String onNewRequest(String method) throws RemoteException {
            Log.d(TAG, "onNewRequest: "+method);
            String[] call=method.split(",");
            String ClassName=call[0];
            String MethodName=call[1];


            if(ClassName.equals("TelephonyManager")&&MethodName.equals("getDeviceId")){
                TelephonyManager telephonyManager= (TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE);
                String IMEI=telephonyManager.getDeviceId();
//            String IMEI="8888";
//            Log.d(TAG, "IMEI: "+IMEI);
                return IMEI;
            }
            if(ClassName.equals("LocationManager")&&MethodName.equals("getLastKnownLocation")){
                return "location";
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
