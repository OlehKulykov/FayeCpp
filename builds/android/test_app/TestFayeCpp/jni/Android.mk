LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := fayecpp-prebuilt
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libfayecpp.so
LOCAL_EXPORT_C_INCLUDES := fayecpp.h
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := testfayecpp
LOCAL_SRC_FILES := testfayecpp.cpp
LOCAL_LDLIBS += -lm -lc -ldl -landroid
LOCAL_SHARED_LIBRARIES += fayecpp-prebuilt

#LOCAL_LDLIBS += -L$(LOCAL_PATH)/$(TARGET_ARCH_ABI)/ -lfayecpp

##LOCAL_SHARED_LIBRARIES += fayecpp
include $(BUILD_SHARED_LIBRARY)
