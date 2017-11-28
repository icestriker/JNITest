//
// Created by Administrator on 2017/11/27.
//

#ifndef JNITEST_TEST_LIB_H
#define JNITEST_TEST_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
#include<string.h>
#include<stdlib.h>
#include <android/log.h>
#define LOG_TAG "Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)


#ifdef __cplusplus
}
#endif
#endif //JNITEST_TEST_LIB_H
