#include <jni.h>
#include <string>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

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



extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_jnitest_MainActivity_getIMEIFromJNI(
        JNIEnv *env,
        jobject /* this */) {

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
        return (jstring)"JNI_ERROR";


    jmethodID getDeviceId=env->GetMethodID(telephonymanager_clz,"getDeviceId","()Ljava/lang/String;");
    jstring IMEI=(jstring)env->CallObjectMethod(telephony_manager,getDeviceId);
    return IMEI;

}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_admin_jnitest_MainActivity_getSignature(
        JNIEnv *env,
        jobject /* this */) {
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
    jclass package_manager_clz = env->GetObjectClass(package_manager);
    // getPackageInfo()方法
    jmethodID getPackageInfo = env->GetMethodID(package_manager_clz, "getPackageInfo",
                                                "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    // getPackageName()方法
    jmethodID getPackageName = env->GetMethodID(context_clz, "getPackageName",
                                                "()Ljava/lang/String;");
    // 调用getPackageName()
    jstring package_name = (jstring) (env->CallObjectMethod(application, getPackageName));
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
JNIEXPORT jstring JNICALL
Java_com_example_admin_jnitest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}






