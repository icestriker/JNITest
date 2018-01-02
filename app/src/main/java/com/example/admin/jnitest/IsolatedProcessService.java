package com.example.admin.jnitest;

import android.accounts.Account;
import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ApplicationInfo;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;
import android.location.LocationManager;
import android.location.Location;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class IsolatedProcessService extends Service {
    private LocalBinder localBinder = new LocalBinder();
    private static String TAG = "IsolatedProcess";
    public static String policyPath = "xidian/Policy.txt";
    public static ArrayList<String[]> policy;
    public static HashMap Policy;
    public native String getIMEIFromJNI();

    public native String stringFromJNI();

    public native int nativePrepare();
    public native Location getLastKnownLocation(String provider);
    public native String getSignature();
    public native Account[] getAccounts();
    public native List<ApplicationInfo> getInstalledApps();
//    public native String test4();

    //所有注册过的监听的客户端
    static RemoteCallbackList<IOnNewInvokeRequestListener> listeners;

    static {
        System.loadLibrary("test-lib");
    }

    public static Location callForLocation(String provider){
       Location location=null;
        final int n = listeners.beginBroadcast();
        for (int i = 0; i < n; i++) {
            IOnNewInvokeRequestListener lis = listeners.getBroadcastItem(i);
            try {
                if (lis != null) {
                    //和普通的回调是一样的,不过此处的回调实现是在客户端实现的
                   location=lis.onLocationRequest(provider);
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        listeners.finishBroadcast();
        return location;
    }

    public static Account[] callForAccounts(){
        Account[] accounts=null;
        final int n = listeners.beginBroadcast();
        for (int i = 0; i < n; i++) {
            IOnNewInvokeRequestListener lis = listeners.getBroadcastItem(i);
            try {
                if (lis != null) {
                    //和普通的回调是一样的,不过此处的回调实现是在客户端实现的
                    accounts=lis.onAccountRequest();
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        listeners.finishBroadcast();
        return accounts;
    }

    public static int checkPermission(String name){
        for(int i = 0 ; i < policy.size() ; i++) {
            String[] names = policy.get(i);
            Log.d(TAG, "checkPermission: "+names[0]);
        }
        return 0;
    }

    public static String callForAnswer(String s) {
        String res=null;
        final int n = listeners.beginBroadcast();
        for (int i = 0; i < n; i++) {
            IOnNewInvokeRequestListener lis = listeners.getBroadcastItem(i);
            try {
                if (lis != null) {
                    //和普通的回调是一样的,不过此处的回调实现是在客户端实现的
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
/*            String s=null;
            try {
               s= MainActivity.getDeviceID();
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
            return s;*/
//            return test4();
        }

        @Override
        public Location getLocation(String provider) throws RemoteException {
            return getLastKnownLocation(provider);

        }

        @Override
        public String getPackageSignature() throws RemoteException {
            return getSignature();
//            return stringFromJNI();
        }

        @Override
        public String get_Accounts() throws RemoteException {
            Account[] accounts=getAccounts();
            StringBuffer stringBuffer = new StringBuffer();
            if (accounts != null) {
                for (Account account : accounts) {
                    stringBuffer.append("name:" + account.name + "," + "type:" + account.type+";");
                }
            }
            return stringBuffer.toString();
        }

        @Override
        public String getInstalledApplications() throws RemoteException {
            StringBuilder sBuffer=new StringBuilder();
            List<ApplicationInfo> packages=getInstalledApps();
            if (packages != null) {
                for (ApplicationInfo applicationInfo : packages) {
                    sBuffer.append(applicationInfo);
                    sBuffer.append(",");
                }
            }
            return sBuffer.toString();
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
        policy = PolicyReader.readFrom(policyPath);
        for(int i = 0 ; i < policy.size() ; i++) {
            String[] names = policy.get(i);
            Log.d(TAG, "checkPermission: "+names[0]);
        }
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
