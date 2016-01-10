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
	../../../libwebsockets/lib/header.c \
	../../../libwebsockets/lib/libwebsockets.c \
	../../../libwebsockets/lib/lws-plat-unix.c \
	../../../libwebsockets/lib/output.c \
	../../../libwebsockets/lib/parsers.c \
	../../../libwebsockets/lib/pollfd.c \
	../../../libwebsockets/lib/service.c \
	../../../libwebsockets/lib/ssl.c \
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
	../../../src/FCAdvice.cpp \
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
	../../../src/REThreadingPrivate.cpp \
	../../../src/REVariant.cpp \
	../../../src/REVariantList.cpp \
	../../../src/REVariantMap.cpp \
	../../../src/REWideString.cpp \
	../../../src/FCTransport.cpp \
	../../../src/FCWebSocket.cpp \
	../getdtablesize.c \
	../../../wolfssl/src/crl.c \
	../../../wolfssl/src/internal.c \
	../../../wolfssl/src/io.c \
	../../../wolfssl/src/keys.c \
	../../../wolfssl/src/ocsp.c \
	../../../wolfssl/src/sniffer.c \
	../../../wolfssl/src/ssl.c \
	../../../wolfssl/src/tls.c \
	../../../wolfssl/wolfcrypt/src/aes.c \
	../../../wolfssl/wolfcrypt/src/arc4.c \
	../../../wolfssl/wolfcrypt/src/asn.c \
	../../../wolfssl/wolfcrypt/src/blake2b.c \
	../../../wolfssl/wolfcrypt/src/camellia.c \
	../../../wolfssl/wolfcrypt/src/chacha.c \
	../../../wolfssl/wolfcrypt/src/coding.c \
	../../../wolfssl/wolfcrypt/src/des3.c \
	../../../wolfssl/wolfcrypt/src/dh.c \
	../../../wolfssl/wolfcrypt/src/dsa.c \
	../../../wolfssl/wolfcrypt/src/ecc.c \
	../../../wolfssl/wolfcrypt/src/error.c \
	../../../wolfssl/wolfcrypt/src/hash.c \
	../../../wolfssl/wolfcrypt/src/hc128.c \
	../../../wolfssl/wolfcrypt/src/hmac.c \
	../../../wolfssl/wolfcrypt/src/integer.c \
	../../../wolfssl/wolfcrypt/src/logging.c \
	../../../wolfssl/wolfcrypt/src/md2.c \
	../../../wolfssl/wolfcrypt/src/md4.c \
	../../../wolfssl/wolfcrypt/src/md5.c \
	../../../wolfssl/wolfcrypt/src/memory.c \
	../../../wolfssl/wolfcrypt/src/misc.c \
	../../../wolfssl/wolfcrypt/src/pkcs7.c \
	../../../wolfssl/wolfcrypt/src/poly1305.c \
	../../../wolfssl/wolfcrypt/src/pwdbased.c \
	../../../wolfssl/wolfcrypt/src/rabbit.c \
	../../../wolfssl/wolfcrypt/src/random.c \
	../../../wolfssl/wolfcrypt/src/ripemd.c \
	../../../wolfssl/wolfcrypt/src/rsa.c \
	../../../wolfssl/wolfcrypt/src/sha.c \
	../../../wolfssl/wolfcrypt/src/sha256.c \
	../../../wolfssl/wolfcrypt/src/sha512.c \
	../../../wolfssl/wolfcrypt/src/tfm.c \
	../../../wolfssl/wolfcrypt/src/wc_encrypt.c \
	../../../wolfssl/wolfcrypt/src/wc_port.c
	


ALL_INCLUDES := \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../../../ \
	$(LOCAL_PATH)/../../../libwebsockets/lib/ \
	$(LOCAL_PATH)/../../../jansson/src/ \
	$(LOCAL_PATH)/../../../jansson/android/ \
	$(LOCAL_PATH)/../../../wolfssl/


ALL_CFLAGS := \
	-w \
	-DHAVE_FAYECPP_CONFIG_H=1 \
	-DHAVE_CONFIG_H=1 \
	-DCMAKE_BUILD=1 \
	-DLWS_BUILTIN_GETIFADDRS=1 \
	-DHAVE_STDINT_H=1 \
	-DRE_HAVE_STDINT_H=1 \
	-DHAVE_ANDROID_LOG_H=1 \
	-DRE_HAVE_ANDROID_LOG_H=1 \
	-DLWS_OPENSSL_SUPPORT=1 \
	-DUSE_WOLFSSL=1


include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(ALL_SOURCES)
LOCAL_C_INCLUDES += $(ALL_INCLUDES)
LOCAL_CFLAGS += $(ALL_CFLAGS)
LOCAL_MODULE := libfayecpp
LOCAL_LDLIBS += -lz -llog
include $(BUILD_SHARED_LIBRARY)

