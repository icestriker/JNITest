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


//向主进程申请函数调用，等待返回结果
jstring callMainInvoke(JNIEnv* env,std::string ClassCode ,std::string MethodCode){
    string paramter="";
    char* s;
//    va_list args;
//    va_start(args,MethodCode);
//if(max){
//    do{
//        s=va_arg(args,char*);
//        LOGD("arg:%s",s);
//        if(s!=NULL)
//            paramter=paramter+s;
//    }
//    while(--max);
//}



//    va_end(args);


  /*  jmethodID connect=mEnv->GetStaticMethodID(LocalSocketClient,"connect","()I");
    mEnv->CallStaticIntMethod(LocalSocketClient,connect);

    jmethodID connect=mEnv->GetStaticMethodID(LocalSocketClient,"connect","()I;");
    jmethodID close=mEnv->GetStaticMethodID(LocalSocketClient,"close","()I");
    jmethodID send=mEnv->GetStaticMethodID(LocalSocketClient,"send","(Ljava/lang/String;)I");
    jmethodID recv=mEnv->GetStaticMethodID(LocalSocketClient,"recv","()Ljava/lang/String;");*/
    jclass LocalSocketClient=env->FindClass("com/example/admin/jnitest/IsolatedProcessService");
    jmethodID callForAnswer=env->GetStaticMethodID(LocalSocketClient,"callForAnswer","(Ljava/lang/String;)Ljava/lang/String;");

    jstring ask=env->NewStringUTF((ClassCode+","+MethodCode+","+paramter).c_str());
    //阻塞式
    jstring  a=(jstring) env->CallStaticObjectMethod(LocalSocketClient, callForAnswer,ask);
    return a;
}

extern  "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_jnitest_IsolatedProcessService_test4(
        JNIEnv *env,
        jobject o /* this */
) {

    return callMainInvoke(env,"a","b");
}


struct MethodId{
    jmethodID getDeviceId;
    jmethodID getLastKnownLocation;
    jmethodID getSystemService;
}Methods;

//jboolean initMethods(JNIEnv* env){
//    Methods.getDeviceId=env->GetMethodID(mEnv->FindClass("android/telephony/TelephonyManager"),"getDeviceId","()Ljava/lang/String;");
//
//}
//
//int getMethodCode(jmethodID m){
//    jmethodID getDeviceId=env->GetMethodID(mEnv->FindClass("android/telephony/TelephonyManager"),"getDeviceId","()Ljava/lang/String;");
//
//    if(m==getDeviceId)
//        return 1;
//    else
//        return 0;
//}


jobject (*CallObjectMethodV_backup)(JNIEnv*, jobject, jmethodID, va_list);
jobject  callObjectMethodV(JNIEnv* env,jobject o,jmethodID m,va_list l){
    if( m==Methods.getDeviceId){
        jstring s=callMainInvoke(env,"TelephonyManager","getDeviceId");
//        jstring s=env->NewStringUTF("8888");
        return s ;
    } else   if(m==Methods.getLastKnownLocation){
        jstring location=callMainInvoke(env,"LocationManager","getLastKnownLocation");
        return location;
    }
    else
        return CallObjectMethodV_backup(env,o,m,l);
}

extern  "C"
JNIEXPORT jint JNICALL
//jint
Java_com_example_admin_jnitest_IsolatedProcessService_nativePrepare(
        JNIEnv *env,
        jobject o /* this */
) {
    Methods.getDeviceId=env->GetMethodID(env->FindClass("android/telephony/TelephonyManager"),"getDeviceId","()Ljava/lang/String;");
//    Methods.getLastKnownLocation=env->GetMethodID(env->FindClass("android/location/LocationManager"),"getProvider","(Ljava/lang/String;)Ljava/lang/Object;");


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
    jobject application = getApplication(env);
    if (application == NULL) {
        return (jstring) JNI_ERR;
    }

    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);
    jclass  telephonymanager_clz=env->FindClass("android/telephony/TelephonyManager");

    jmethodID getSystemService = env->GetMethodID(context_clz, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jstring TELEPHONY_SERVICE=env->NewStringUTF("phone");
    jobject telephony_manager = env->CallObjectMethod(application, getSystemService,TELEPHONY_SERVICE);
    if(telephony_manager==NULL)
        return (jstring)JNI_ERR;


    jmethodID getDeviceId=env->GetMethodID(telephonymanager_clz,"getDeviceId","()Ljava/lang/String;");

   jstring  IMEI=(jstring)env->CallObjectMethod(telephony_manager,getDeviceId);
    if(IMEI==NULL)
        IMEI=env->NewStringUTF("error");
//   const char* s=env->GetStringUTFChars(IMEI,JNI_FALSE);
//    LOGD("IMEI:%s",s);
    return IMEI;
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

