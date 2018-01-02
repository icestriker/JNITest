package com.example.admin.jnitest;

import java.util.HashMap;

/**
 * Created by Administrator on 2017/12/22.
 */

public class PermissionChecker {
    public  HashMap<String,Integer> permission;

    public PermissionChecker(){
        permission=new HashMap<String,Integer>();
        permission.put("phone",1);
        permission.put("location",1);
        permission.put("package",1);
        permission.put("account",1);
    }
    public  int checkPermission(String name){
        return permission.get(name);
    }
}
