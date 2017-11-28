package com.example.admin.jnitest;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

public class IsolatedProcessService extends Service {
    private LocalBinder localBinder=new LocalBinder();

    static {
        System.loadLibrary("test-lib");
    }

    public native String  getIMEIFromJNI();
    public native String  stringFromJNI();
    public native int nativePrepare();
    public native String test4();

    public class LocalBinder extends  IServiceInterface.Stub{
        @Override
        public String getIMEI() throws RemoteException {
            return getIMEIFromJNI();
        }

        @Override
        public int prepare() throws RemoteException {
            return nativePrepare();
        }

        @Override
        public String getString() throws RemoteException {
            return stringFromJNI();
        }


        @Override
        public IBinder asBinder() {
            return null;
        }
    }

    public IsolatedProcessService() {
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
       return  localBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        return super.onUnbind(intent);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d("IPS", "onDestroy: ");
    }
}
