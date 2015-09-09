#ifndef __COAP_JNI_H__
#define __COAP_JNI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>

JNIEXPORT jint JNICALL Java_org_iot4free_coap_CoapClient_coap_1recv      (JNIEnv * env, jobject obj, jint session_index, jbyteArray buf, jint len);
JNIEXPORT jint JNICALL Java_org_iot4free_coap_CoapClient_coap_1send      (JNIEnv * env, jobject obj, jint session_index, jbyteArray method, jbyteArray url, jbyteArray buf, jint len);
JNIEXPORT jint JNICALL Java_org_iot4free_coap_CoapClient_coap_1connect   (JNIEnv * env, jobject obj, jbyteArray buf, jint port);
JNIEXPORT jint JNICALL Java_org_iot4free_coap_CoapClient_coap_1close     (JNIEnv * env, jobject obj, jint session_index);
JNIEXPORT jint JNICALL Java_org_iot4free_coap_CoapClient_coap_1setsockopt(JNIEnv * env, jobject obj, jint session_index, jint level, jint optname, jbyteArray optval, jint optlen);
JNIEXPORT jint JNICALL Java_org_iot4free_coap_CoapClient_coap_1getsockopt(JNIEnv * env, jobject obj, jint session_index, jint level, jint optname, jbyteArray optval);

#ifdef __cplusplus
}
#endif

#endif
