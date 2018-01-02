//
// Created by Administrator on 2017/10/30.
//
#include "test-lib.h"
#include <jni.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sysconf.h>

using namespace std;

string TYPE_STRING="string";

//获取当前进程的Context
static jobject getApplication(JNIEnv *env) {
    jobject application = NULL;
    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        } else {

        }
        env->DeleteLocalRef(activity_thread_clz);
    }

    return application;
}

jint checkPermission(JNIEnv* env,string name){
    jclass PermissionChecker=env->FindClass("com/example/admin/jnitest/PermissionChecker");
    jmethodID init=env->GetMethodID(PermissionChecker,"<init>","()V");
    jobject permissionchecker=env->NewObject(PermissionChecker,init);

    jmethodID getPermission=env->GetMethodID(PermissionChecker,"checkPermission","(Ljava/lang/String;)I");
    jstring SERVICE=env->NewStringUTF(name.c_str());
    return env->CallIntMethod(permissionchecker,getPermission,SERVICE);
}

//向主进程申请函数调用，等待返回结果
jstring callObjectInvoke(JNIEnv* env,std::string ClassCode ,std::string MethodCode,string args){

    /*  jmethodID connect=mEnv->GetStaticMethodID(LocalSocketClient,"connect","()I");
      mEnv->CallStaticIntMethod(LocalSocketClient,connect);

      jmethodID connect=mEnv->GetStaticMethodID(LocalSocketClient,"connect","()I;");
      jmethodID close=mEnv->GetStaticMethodID(LocalSocketClient,"close","()I");
      jmethodID send=mEnv->GetStaticMethodID(LocalSocketClient,"send","(Ljava/lang/String;)I");
      jmethodID recv=mEnv->GetStaticMethodID(LocalSocketClient,"recv","()Ljava/lang/String;");*/
    jclass LocalSocketClient=env->FindClass("com/example/admin/jnitest/IsolatedProcessService");
    jmethodID callForAnswer=env->GetStaticMethodID(LocalSocketClient,"callForAnswer","(Ljava/lang/String;)Ljava/lang/String;");

//    LOGD("ask:%s",(ClassCode+","+MethodCode+","+paramter).c_str());
    jstring ask=env->NewStringUTF((ClassCode+","+MethodCode+","+args).c_str());
    //阻塞式
    jstring  a=(jstring) env->CallStaticObjectMethod(LocalSocketClient, callForAnswer,ask);
    return a;
}

//向主进程申请函数调用，等待返回结果
jstring callStringInvoke(JNIEnv* env,std::string ClassCode ,std::string MethodCode,int max,va_list args){
    string paramter="";
    char* s;
    int i;
    if(!MethodCode.compare("getDeviceId")){
        while(max--)
        {
            i=va_arg(args,int);
            sprintf(s,"%d",i);

            LOGD("arg:%s",s);

            paramter=paramter+s;
            LOGD("paramter:%s",paramter.c_str());
        }
        LOGD("while break");
    }

    jclass LocalSocketClient=env->FindClass("com/example/admin/jnitest/IsolatedProcessService");
    jmethodID callForAnswer=env->GetStaticMethodID(LocalSocketClient,"callForAnswer","(Ljava/lang/String;)Ljava/lang/String;");

//    LOGD("ask:%s",(ClassCode+","+MethodCode+","+paramter).c_str());
//    char* num;
//    sprintf(num,"%d",max);
    jstring ask=env->NewStringUTF((ClassCode+","+MethodCode+","+paramter).c_str());
    //阻塞式
    jstring  a=(jstring) env->CallStaticObjectMethod(LocalSocketClient, callForAnswer,ask);
    return a;
}
//
//extern  "C"
//JNIEXPORT jstring JNICALL
//Java_com_example_admin_jnitest_IsolatedProcessService_test4(
//        JNIEnv *env,
//        jobject o /* this */
//) {
//    jstring location=callMainInvoke(env,"LocationManager","getLastKnownLocation",1,l);
//    return location;
//}


struct MethodId{
    jmethodID getDeviceId;
    jmethodID getDeviceId1;
    jmethodID getPackageInfo;
    jmethodID getPackageName;
    jmethodID getLastKnownLocation;//未完成
    jmethodID getInstalledApplications;
    jmethodID getAccounts;
}Methods;

void initMethods(JNIEnv* env){
    Methods.getDeviceId=env->GetMethodID(env->FindClass("android/telephony/TelephonyManager"),"getDeviceId","()Ljava/lang/String;");
    Methods.getDeviceId1=env->GetMethodID(env->FindClass("android/telephony/TelephonyManager"),"getDeviceId","(I)Ljava/lang/String;");
    Methods.getLastKnownLocation=env->GetMethodID(env->FindClass("android/location/LocationManager"),"getLastKnownLocation","(Ljava/lang/String;)Landroid/location/Location;");
    Methods.getPackageName = env->GetMethodID(env->FindClass("android/content/Context"), "getPackageName","()Ljava/lang/String;");
    Methods.getPackageInfo = env->GetMethodID(env->FindClass("android/content/pm/PackageManager"), "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    Methods.getAccounts = env->GetMethodID(env->FindClass("android/accounts/AccountManager"),"getAccounts","()[Landroid/accounts/Account;");
    Methods.getInstalledApplications=env->GetMethodID(env->FindClass("android/content/pm/PackageManager"),"getInstalledApplications","(I)Ljava/util/List;");
}



void hook(jmethodID m){

}

jobject (*CallObjectMethodV_backup)(JNIEnv*, jobject, jmethodID, va_list);
//jobject  (*CallObjectMethodA_backup)(JNIEnv*, jobject, jmethodID, jvalue*);
jobject  callObjectMethodV(JNIEnv* env,jobject o,jmethodID m,va_list l){
    if( m==Methods.getDeviceId && checkPermission(env,"phone")){
        LOGD("getDeviceId");
        jstring s=callStringInvoke(env,"TelephonyManager","getDeviceId",0,l);
        return s ;
    } else   if(m==Methods.getLastKnownLocation && checkPermission(env,"location")){
        jclass LocalSocketClient=env->FindClass("com/example/admin/jnitest/IsolatedProcessService");
        jmethodID callForAnswer=env->GetStaticMethodID(LocalSocketClient,"callForLocation","(Ljava/lang/String;)Landroid/location/Location;");

        jstring ask=env->NewStringUTF("gps");
        //阻塞式
        jobject  a= env->CallStaticObjectMethod(LocalSocketClient, callForAnswer,ask);

        return a;
    }
    else if (m==Methods.getDeviceId1&&checkPermission(env,"phone")){
        jstring r=callStringInvoke(env,"TelephonyManager","getDeviceId",1,l);
        return r;
    }
    else if(m==Methods.getPackageInfo&&checkPermission(env,"package")){
        LOGD("Detect getPackageInfo");
        return CallObjectMethodV_backup(env,o,m,l);
    }
    else if(m==Methods.getAccounts&& checkPermission(env,"account")){
        LOGD("Detect getAccounts");
        jclass LocalSocketClient=env->FindClass("com/example/admin/jnitest/IsolatedProcessService");
        jmethodID callForAccounts=env->GetStaticMethodID(LocalSocketClient,"callForAccounts","()[Landroid/accounts/Account;");
        //阻塞式
        jobjectArray accounts=(jobjectArray) env->CallStaticObjectMethod(LocalSocketClient, callForAccounts);

        return accounts;
    }
    else
        return CallObjectMethodV_backup(env,o,m,l);
}



extern "C"
JNIEXPORT jint JNICALL
//jint
Java_com_example_admin_jnitest_IsolatedProcessService_nativePrepare(
        JNIEnv *env,
        jobject o /* this */
) {
    initMethods(env);
    LOGD("env modifying");
    //改写env
    int  page_size = sysconf(_SC_PAGE_SIZE);
    void* p = (void*) (( (size_t)env->functions) & ~(page_size-1));//
    int res=mprotect(p,2*page_size,PROT_READ | PROT_WRITE);

    if(res==0){
        CallObjectMethodV_backup=env->functions->CallObjectMethodV;
        env->functions->CallObjectMethodV=callObjectMethodV;
        return 0;
    }
    else return -1;
}


extern  "C"
JNIEXPORT jstring JNICALL
//jstring
Java_com_example_admin_jnitest_IsolatedProcessService_getIMEIFromJNI(
        JNIEnv *env,
        jobject /* this */
       ) {
  /*  jclass MainActivity=env->FindClass("com/example/admin/jnitest/MainActivity");
    jmethodID getDeviceId=env->GetStaticMethodID(MainActivity,"getDeviceID","()Ljava/lang/String;");
    jstring IMEI=(jstring)env->CallStaticObjectMethod(MainActivity,getDeviceId);
    return IMEI;*/
    jobject application = getApplication(env);
    if (application == NULL) {
        return (jstring) JNI_ERR;
    }
    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);

    jmethodID getSystemService = env->GetMethodID(context_clz, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jstring TELEPHONY_SERVICE=env->NewStringUTF("phone");
    jobject telephony_manager = env->CallObjectMethod(application, getSystemService,TELEPHONY_SERVICE);
    if(telephony_manager==NULL) {
        return (jstring)JNI_ERR;
    }


    jclass  telephonymanager_clz=env->FindClass("android/telephony/TelephonyManager");
    jmethodID getDeviceId=env->GetMethodID(telephonymanager_clz,"getDeviceId","(I)Ljava/lang/String;");
   jmethodID  getDeviceId1=env->GetMethodID(env->FindClass("android/telephony/TelephonyManager"),"getDeviceId","(I)Ljava/lang/String;");
    if(getDeviceId==getDeviceId1)
        LOGD("getDeviceID equal");
    else
        LOGD("getDeviceId unequal");
   jstring  IMEI=(jstring)env->CallObjectMethod(telephony_manager,getDeviceId,0);
    if(IMEI==NULL)
        IMEI=env->NewStringUTF("error");
//   const char* s=env->GetStringUTFChars(IMEI,JNI_FALSE);
//    LOGD("IMEI:%s",s);
    return IMEI;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_admin_jnitest_IsolatedProcessService_getLastKnownLocation(JNIEnv *env,
                                                                           jobject instance,jstring provider) {
    jobject application = getApplication(env);
    if (application == NULL) {
        return (jstring) JNI_ERR;
    }
    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);

    jmethodID getSystemService = env->GetMethodID(context_clz, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jstring LOCATION_SERVICE=env->NewStringUTF("location");
    jobject location_manager = env->CallObjectMethod(application, getSystemService,LOCATION_SERVICE);
    if(location_manager==NULL) {
        return (jstring)JNI_ERR;
    }
    jclass location_manager_clazz=env->GetObjectClass(location_manager);
    jmethodID getLastKnownLocation=env->GetMethodID(location_manager_clazz,"getLastKnownLocation","(Ljava/lang/String;)Landroid/location/Location;");
    jstring GPS=env->NewStringUTF("gps");
 /*   std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());*/
    jobject location=env->CallObjectMethod(location_manager,getLastKnownLocation,GPS);
    return location;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_jnitest_IsolatedProcessService_getSignature(JNIEnv *env, jobject instance) {
    // Application object
    jobject application = getApplication(env);
    if (application == NULL) {
        return (jstring) JNI_ERR;
    }
    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);
    // getPackageManager()方法
    jmethodID getPackageManager = env->GetMethodID(context_clz, "getPackageManager",
                                                   "()Landroid/content/pm/PackageManager;");
    // 获取PackageManager实例
    jobject package_manager = env->CallObjectMethod(application, getPackageManager);
    // PackageManager class
    jclass package_manager_clz = env->FindClass("android/content/pm/PackageManager");
    // getPackageInfo()方法
    jmethodID getPackageInfo = env->GetMethodID(package_manager_clz, "getPackageInfo",
                                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    // getPackageName()方法
    jmethodID getPackageName = env->GetMethodID(context_clz, "getPackageName",
                                                "()Ljava/lang/String;");
    // 调用getPackageName()
    jstring package_name = (jstring) (env->CallObjectMethod(application, getPackageName));
   jmethodID getPackageInfo1 = env->GetMethodID(env->FindClass("android/content/pm/PackageManager"), "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    if(getPackageInfo==getPackageInfo1)
        LOGD("getPackageInfo equal");
    else
        LOGD("getPackageInfo unequal");
    // PackageInfo实例
    jobject package_info = env->CallObjectMethod(package_manager, getPackageInfo, package_name, 64);
    // PackageInfo class
    jclass package_info_clz = env->GetObjectClass(package_info);
    // signatures字段
    jfieldID signatures_field = env->GetFieldID(package_info_clz, "signatures",
                                                "[Landroid/content/pm/Signature;");
    jobject signatures = env->GetObjectField(package_info, signatures_field);
    jobjectArray signatures_array = (jobjectArray) signatures;
    jobject signature0 = env->GetObjectArrayElement(signatures_array, 0);
    // Signature class
    jclass signature_clz = env->GetObjectClass(signature0);
    // toCharsString()方法
    jmethodID toCharsString = env->GetMethodID(signature_clz, "toCharsString",
                                               "()Ljava/lang/String;");
    // 调用toCharsString()
    jstring signature_str = (jstring) (env->CallObjectMethod(signature0, toCharsString));

    return signature_str;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_example_admin_jnitest_IsolatedProcessService_getAccounts(JNIEnv *env, jobject instance) {
    jobject application = getApplication(env);

    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);
    jmethodID getSystemService = env->GetMethodID(context_clz, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jstring ACCOUNT_SERVICE=env->NewStringUTF("account");
    jobject account_manager = env->CallObjectMethod(application, getSystemService,ACCOUNT_SERVICE);

    jclass account_manager_clazz=env->GetObjectClass(account_manager);
    jmethodID getAccounts=env->GetMethodID(account_manager_clazz,"getAccounts","()[Landroid/accounts/Account;");

    jobjectArray accounts=(jobjectArray)env->CallObjectMethod(account_manager,getAccounts);
    return accounts;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_admin_jnitest_IsolatedProcessService_getInstalledApps(JNIEnv *env,
                                                                       jobject instance) {
    // Application object
    jobject application = getApplication(env);
    if (application == NULL) {
        return (jstring) JNI_ERR;
    }
    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);
    // getPackageManager()方法
    jmethodID getPackageManager = env->GetMethodID(context_clz, "getPackageManager",
                                                   "()Landroid/content/pm/PackageManager;");
    // 获取PackageManager实例
    jobject package_manager = env->CallObjectMethod(application, getPackageManager);

    // PackageManager class

    jclass package_manager_clz = env->FindClass("android/content/pm/PackageManager");
    jmethodID getInstalledApplications=env->GetMethodID(package_manager_clz,"getInstalledApplications","(I)Ljava/util/List;");
    jobject installed=env->CallObjectMethod(package_manager,getInstalledApplications,0);
    return installed;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_jnitest_IsolatedProcessService_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

/*
static const JNINativeMethod gMethods[] ={
        {"test1","()I",(void*)Java_com_example_admin_jnitest_IsolatedProcessService_test1},
        {"getIMEIFromJNI","()Ljava/lang/String;",(void*)Java_com_example_admin_jnitest_IsolatedProcessService_getIMEIFromJNI}
};
jint
JNI_OnLoad(JavaVM* vm, void* reserved){
    JNIEnv* env = NULL;
    jint result = -1;

    LOGD("JNI_Onload");
    jint ver=vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if(ver!=JNI_OK)
        return -1;


    //注册函数
    jclass  IsolatedProcessService=env->FindClass("com/example/admin/jnitest/IsolatedProcessService");
    if((*env).RegisterNatives(IsolatedProcessService,gMethods,2)<0)
        return -1;


    result=JNI_VERSION_1_6;
    return result;
}
*/

