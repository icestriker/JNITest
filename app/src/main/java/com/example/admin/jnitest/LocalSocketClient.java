package com.example.admin.jnitest;

import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;

/**
 * Created by Administrator on 2017/11/17.
 */

public class LocalSocketClient {
    private static final String TAG = "Client:";
    private static final String name = "com.repackaging.localsocket";
    private static LocalSocket Client = null;
    private static PrintWriter os = null;
    private static BufferedReader is = null;
    private static int timeout = 30000;


    public static String callForAnswer(String a){
        LocalSocketClient.connect();
        LocalSocketClient.send(a);

        String b=LocalSocketClient.recv();
        LocalSocketClient.close();
        return b;
    }

    public static int connect(){
        try {
            Client = new LocalSocket();
            Client.connect(new LocalSocketAddress(name));
            Client.setSoTimeout(timeout);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 1;
    }

    public static int  send(String data) {
        try {
            os = new PrintWriter(Client.getOutputStream());
//            for(int i = 0 ; i < data.length ; i ++){
//                os.println(data[0]);
//            }
            os.println(data);
            os.flush();
            Log.d(TAG,"Client send completed");
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 1;
    }

    public static String recv() {
        String result = null;
        try {
            is = new BufferedReader(new InputStreamReader(Client.getInputStream()));
            result = is.readLine();
            Log.d(TAG,"client recved:"+ result);
            if(result==null)
                result="error";
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
        }
        return result;
    }

    public static int  close() {
        try {
            is.close();
            os.close();
            Client.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 1;
    }
}
