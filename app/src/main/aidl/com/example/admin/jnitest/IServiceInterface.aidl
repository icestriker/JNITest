// IServiceInterface.aidl
package com.example.admin.jnitest;

// Declare any non-default types here with import statements
import com.example.admin.jnitest.IOnNewInvokeRequestListener;
interface IServiceInterface {
     String getString();
     String getIMEI();
     int prepare();
       void  registerOnNewRequestArrivedListener(in IOnNewInvokeRequestListener listener);
            void  unregisterOnNewRequestArrivedListener(in IOnNewInvokeRequestListener listener);
}
