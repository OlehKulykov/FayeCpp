LOCAL_PATH:= $(call my-dir)


ALL_SOURCES := \
	../../../libwebsockets/lib/alloc.c \
	../../../libwebsockets/lib/base64-decode.c \
	../../../libwebsockets/lib/client-handshake.c \
	../../../libwebsockets/lib/client-parser.c \
	../../../libwebsockets/lib/client.c \
	../../../libwebsockets/lib/context.c \
	../../../libwebsockets/lib/extension-deflate-frame.c \
	../../../libwebsockets/lib/extension-deflate-stream.c \
	../../../libwebsockets/lib/getifaddrs.c \
	../../../libwebsockets/lib/extension.c \
	../../../libwebsockets/lib/handshake.c \
	../../../libwebsockets/lib/libwebsockets.c \
	../../../libwebsockets/lib/lws-plat-unix.c \
	../../../libwebsockets/lib/output.c \
	../../../libwebsockets/lib/parsers.c \
	../../../libwebsockets/lib/pollfd.c \
	../../../libwebsockets/lib/service.c \
	../../../libwebsockets/lib/sha-1.c \
	../../../jansson/src/dump.c \
	../../../jansson/src/error.c \
	../../../jansson/src/hashtable.c \
	../../../jansson/src/hashtable_seed.c \
	../../../jansson/src/load.c \
	../../../jansson/src/memory.c \
	../../../jansson/src/pack_unpack.c \
	../../../jansson/src/strbuffer.c \
	../../../jansson/src/strconv.c \
	../../../jansson/src/utf.c \
	../../../jansson/src/value.c \
	../../../src/FCClient.cpp \
	../../../src/FCError.cpp \
	../../../src/FCJsonUtils.cpp \
	../../../src/FCMessage.cpp \
	../../../src/REBuffer.cpp \
	../../../src/REBufferNoCopy.cpp \
	../../../src/RELog.cpp \
	../../../src/REMutableString.cpp \
	../../../src/REStaticString.cpp \
	../../../src/REString.cpp \
	../../../src/REStringBase.cpp \
	../../../src/REStringList.cpp \
	../../../src/REStringUtilsPrivate.cpp \
	../../../src/REVariant.cpp \
	../../../src/REVariantList.cpp \
	../../../src/REVariantMap.cpp \
	../../../src/REWideString.cpp \
	../../../src/FCTransport.cpp \
	../../../src/FCWebSocket.cpp \
	../getdtablesize.c


ALL_INCLUDES := \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../../../ \
	$(LOCAL_PATH)/../../../libwebsockets/lib/ \
	$(LOCAL_PATH)/../../../jansson/src/ \
	$(LOCAL_PATH)/../../../jansson/android/


ALL_CFLAGS := \
	-w \
	-DHAVE_FAYECPP_CONFIG_H=1 \
	-DHAVE_CONFIG_H=1 \
	-DCMAKE_BUILD=1 \
	-DLWS_BUILTIN_GETIFADDRS=1 \
	-DHAVE_STDINT_H=1 \
	-DHAVE_GETIFADDRS=0 \
	-DHAVE_ANDROID_LOG_H=1


include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(ALL_SOURCES)
LOCAL_C_INCLUDES += $(ALL_INCLUDES)
LOCAL_CFLAGS += $(ALL_CFLAGS)
LOCAL_MODULE := libfayecpp
LOCAL_LDLIBS += -lz -llog
include $(BUILD_SHARED_LIBRARY)

