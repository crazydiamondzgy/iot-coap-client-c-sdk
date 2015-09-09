LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../inc
LOCAL_CFLAGS := -DLINUX
LOCAL_ARM_MODE := arm
LOCAL_CPPFLAGS := -g -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-error=format-security -DLINUX -finline-functions -fexceptions -fno-strict-aliasing
LOCAL_CFLAGS := -g -fPIC -Wall -Wextra -Wno-unused-parameter -Wno-error=format-security -DLINUX -finline-functions -fexceptions -fno-strict-aliasing
LOCAL_LDLIBS :=-L$(SYSROOT)/usr/lib -llog
LOCAL_CPP_EXTENSION :=.cpp

LOCAL_MODULE := ucoap
LOCAL_SRC_FILES :=coap_jni.cpp ../../../src/ucoap.c ../../../src/coap_endpoint.c ../../../src/coap_log.c ../../../src/coap_packet.c ../../../src/coap_util.c ../../../src/mutex.c

include $(BUILD_SHARED_LIBRARY)
