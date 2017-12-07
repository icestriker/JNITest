package com.example.admin.jnitest;

import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

public class IsolatedProcessService extends Service {
    private LocalBinder localBinder = new LocalBinder();
    private static String TAG = "IsolatedProcess";

    public native String getIMEIFromJNI();

    public native String stringFromJNI();

    public native int nativePrepare();

    public native String test4();

    //所有注册过的监听的客户端
    static RemoteCallbackList<IOnNewInvokeRequestListener> listeners;

    static {
        System.loadLibrary("test-lib");
    }

    public static String callForAnswer(String s) {
        String res=null;
        final int n = listeners.beginBroadcast();
        for (int i = 0; i < n; i++) {
            IOnNewInvokeRequestListener lis = listeners.getBroadcastItem(i);
            try {
                if (lis != null) {
                    //和普通的回调是一样的,不过此处的回调实现是在客户端实现的
                    //通知所有注册过的客户端,有新书
                    res=lis.onNewRequest(s);
                    Log.d(TAG, "callForAnswer: "+res);
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        listeners.finishBroadcast();
        return res;
    }

    public class LocalBinder extends IServiceInterface.Stub {
        @Override
        public String getIMEI() throws RemoteException {
            return getIMEIFromJNI();

//            return test4();
        }

        @Override
        public int prepare() throws RemoteException {
            return nativePrepare();
        }

        @Override
        public void registerOnNewRequestArrivedListener(IOnNewInvokeRequestListener listener) throws RemoteException {
            listeners.register(listener);
        }

        @Override
        public void unregisterOnNewRequestArrivedListener(IOnNewInvokeRequestListener listener) throws RemoteException {
            listeners.register(listener);
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
        listeners = new RemoteCallbackList<>();
        return localBinder;
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
