#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include "ucoap.h"
#include "coap_jni.h"

jint Java_org_iot4free_coap_CoapClient_coap_1recv
(
	JNIEnv * env, 
	jobject obj, 
	jint session_index, 
	jbyteArray buf, 
	jint len
)
{
	jbyte * p = env->GetByteArrayElements(buf, 0);
	int ret = coap_recv(session_index, (char *)p, len);
	env->ReleaseByteArrayElements(buf, p, 0);

	return (jint)ret;
}

jint Java_org_iot4free_coap_CoapClient_coap_1send
(
	JNIEnv * env, 
	jobject obj, 
	jint session_index, 
	jbyteArray method, 
	jbyteArray url, 
	jbyteArray buf, 
	jint len
)
{
	jbyte * p_method = env->GetByteArrayElements(method, 0);
	jbyte * p_url = env->GetByteArrayElements(url, 0);
	jbyte * p_buf = env->GetByteArrayElements(buf, 0);
	int ret = coap_send(session_index, (char *)p_method, (char *)p_url, (char *)p_buf, len);
	env->ReleaseByteArrayElements(buf, p_buf, 0);
	env->ReleaseByteArrayElements(buf, p_url, 0);
	env->ReleaseByteArrayElements(buf, p_method, 0);

	return (jint)ret;
}

jint Java_org_iot4free_coap_CoapClient_coap_1connect
(
	JNIEnv * env, 
	jobject obj, 
	jbyteArray buf, 
	jint port
)
{
	jbyte * p = env->GetByteArrayElements(buf, 0);
	int session_index = coap_connect((char *)p, port);
	env->ReleaseByteArrayElements(buf, p, 0);
	
	return (jint)session_index;
}

jint Java_org_iot4free_coap_CoapClient_coap_1close
(
	JNIEnv * env, 
	jobject obj, 
	jint session_index
)
{
	return (jint)coap_close(session_index);
}

jint Java_org_iot4free_coap_CoapClient_coap_1setsockopt
(
	JNIEnv * env, 
	jobject obj, 
	jint session_index, 
	jint level, 
	jint optname, 
	jbyteArray optval, 
	jint optlen
)
{
	jbyte * p_optval = env->GetByteArrayElements(optval, 0);
	int ret = coap_setsockopt(session_index, level, optname, p_optval, optlen);
	env->ReleaseByteArrayElements(optval, p_optval, 0);
	
	return ret;

}

jint Java_org_iot4free_coap_CoapClient_coap_1getsockopt
(
	JNIEnv * env, 
	jobject obj, 
	jint session_index, 
	jint level, 
	jint optname, 
	jbyteArray optval
)
{
	jbyte * p_optval = env->GetByteArrayElements(optval, 0);
	int ret = coap_getsockopt(session_index, level, optname, p_optval);
	env->ReleaseByteArrayElements(optval, p_optval, 0);
	
	return ret;
}
