// IServiceInterface.aidl
package com.example.admin.jnitest;

// Declare any non-default types here with import statements
import com.example.admin.jnitest.IOnNewInvokeRequestListener;
import android.location.Location;
interface IServiceInterface {
     String getString();
     String getIMEI();
     Location getLocation(in String provider);
     String getPackageSignature();
     String get_Accounts();
     String getInstalledApplications();
     int prepare();
     void  registerOnNewRequestArrivedListener(in IOnNewInvokeRequestListener listener);
     void  unregisterOnNewRequestArrivedListener(in IOnNewInvokeRequestListener listener);
}
