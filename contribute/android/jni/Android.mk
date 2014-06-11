LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := 	../../../client.cpp \
					../../../message.cpp \
					../../../variant.cpp \
					../../../private/jsonutils.cpp \
					../../../private/REBuffer.cpp \
					../../../private/REMutex.cpp \
					../../../private/REThread.cpp \
					../../../private/transport.cpp \
					../../../private/websocket.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../../ \
					$(LOCAL_PATH)/../../../libwebsockets/lib/ \
					$(LOCAL_PATH)/../../../jansson/src/ \
					$(LOCAL_PATH)/../../../jansson/android/
		
LOCAL_CFLAGS :=  -Werror -DHAVE_FAYECPP_CONFIG_H=1

LOCAL_LDLIBS := -llog -lz

LOCAL_MODULE := libfayecpp

include $(BUILD_SHARED_LIBRARY)
