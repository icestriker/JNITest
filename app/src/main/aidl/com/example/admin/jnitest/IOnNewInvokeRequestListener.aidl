// IOnNewInvokeRequestListener.aidl
package com.example.admin.jnitest;

// Declare any non-default types here with import statements
import android.accounts.Account;
interface IOnNewInvokeRequestListener {
    String onNewRequest(in String method);
    Account[] onAccountRequest();
    Location onLocationRequest(in String provider);
}
