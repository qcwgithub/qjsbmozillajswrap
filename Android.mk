# Android.mk
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libjs_static
LOCAL_SRC_FILES := $(LOCAL_PATH)/spidermonkey/prebuilt/android/armeabi-v7a/libjs_static.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := mozjswrap.cpp
        
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/spidermonkey/include/android
LOCAL_STATIC_LIBRARIES := libjs_static
# LOCAL_SHARED_LIBRARIES := \
# libpi_lib \
# libpi_math \
# libpi_mesh \
# libglloader
    
# LOCAL_CFLAGS := -Wall -Wno-switch -Wno-unused-function -Wno-unused-value -Istdc++
LOCAL_CPPFLAGS += -D__STDC_LIMIT_MACROS=1 -Wno-invalid-offsetof
LOCAL_EXPORT_CPPFLAGS := -D__STDC_LIMIT_MACROS=1 -Wno-invalid-offsetof
LOCAL_LDLIBS += -lz -llog -ldl -landroid
#LOCAL_LDLIBS += /home/qiucw/Desktop/ndk_Dir/sources/cxx-stl/stlport/libs/armeabi-v7a/libstlport_static.a
#LOCAL_LDLIBS += -lz -llog -ldl
LOCAL_MODULE:= mozjswrap
include $(BUILD_SHARED_LIBRARY)
