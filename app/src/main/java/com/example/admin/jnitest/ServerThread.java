package com.example.admin.jnitest;

import android.content.Context;
import android.net.Credentials;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.nfc.Tag;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.util.Log;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.lang.reflect.*;
import java.util.logging.Handler;
import java.util.logging.LogRecord;

import static android.content.ContentValues.TAG;
import static java.lang.Thread.sleep;

/**
 * Created by Administrator on 2017/11/14.
 */

public class ServerThread implements Runnable {
    public static final String SOCKET_NAME = "com.repackaging.localsocket";
    private static final String TAG = "SocketService";
    private LocalServerSocket mServerSocket = null;
    private Context mConetxt;


    private String invoke(String ClassName,String MethodName,String... args){
        if(ClassName.equals("TelephonyManager")&&MethodName.equals("getDeviceId")){
            TelephonyManager telephonyManager= (TelephonyManager) mConetxt.getSystemService(Context.TELEPHONY_SERVICE);
            String IMEI=telephonyManager.getDeviceId();
//            String IMEI="8888";
//            Log.d(TAG, "IMEI: "+IMEI);
            return IMEI;
        }
        return "ERROR";
    }

    private void getmConetxt(){
        try {
            Class activity_thread=Class.forName("android.app.ActivityThread");
            java.lang.reflect.Method method= activity_thread.getMethod("currentApplication");
            mConetxt= (Context) method.invoke(null);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        getmConetxt();
        try {
            mServerSocket = new LocalServerSocket(SOCKET_NAME);
            while (true) {
                Log.d(TAG, "Server:waiting for connect: ");
                LocalSocket socket = mServerSocket.accept();
                Credentials cre = socket.getPeerCredentials();
                Log.i(TAG, "accept socket uid:" + cre.getUid());

                if (socket != null) {
                    InputStream is = socket.getInputStream();
                    OutputStream os = socket.getOutputStream();

                    InputStreamReader isr = new InputStreamReader(is);

                    char[] data = new char[128];
                    BufferedReader reader = new BufferedReader(isr);
                    String s = reader.readLine();
                    Log.d(TAG, "Server: received" + s);
                    String res=null;
                    if(s!=null&&!s.isEmpty()){
                        String[] call=s.split(",");
                        res=invoke(call[0],call[1]);

//                        res="answer";
                    }


                    Log.d(TAG, "Server:start to send ");
                    PrintWriter writer = new PrintWriter(os);

                    if(res!=null)
                        writer.println(res);
                    else
                        writer.println("error");
                    writer.flush();

                    Log.v(TAG, "Server Sended");
                }
                socket.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

}
