#!/bin/bash
echo "generating android project files"

export ANDROID_BUILD_TOP="."

python ./gyp/gyp_main.py --toplevel-dir=. --depth=. ./moz.gyp -f android-make -G android_ndk_version=r9d

mv ./GypAndroid.mk ./Android.mk

sed -i '10a include $(LOCAL_PATH)/prebuilt.mk' Android.mk

ndk-build NDK_MODULE_PATH=. NDK_PROJECT_PATH=.. NDK_DEBUG=0 NDK_LOG=1 V=1 2>&1|tee build.log 