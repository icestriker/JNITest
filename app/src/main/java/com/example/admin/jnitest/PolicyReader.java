package com.example.admin.jnitest;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

/**
 * Created by Administrator on 2017/12/21.
 */

public class PolicyReader {
    /*Get the policy in file path*/
    public static ArrayList<String[]> readFrom(String path){
        ArrayList<String[]> result = new ArrayList<String[]>();
        int totalCount = 0;
        int errorCount = 0;
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(new File(path)),
                    "UTF-8"));
            String lineTxt = null;
            while ((lineTxt = br.readLine()) != null) {
                totalCount++;
                lineTxt = lineTxt.replace(" ","");
                String[] names = lineTxt.split(",");
                if(names.length!=2){
                    errorCount++;
                    continue;
                }
                result.add(names);
            }
            System.out.println("Read policy file \""+path+"\" finished, total "+totalCount+" Lines, error "+errorCount+" Lines");
            br.close();
        } catch (Exception e) {
            System.err.println("Read policy file errors: " + e);
        }
        return result;
    }
}
