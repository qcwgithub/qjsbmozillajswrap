# Application.mk
APP_MODULES := mozjswrap
APP_ABI := armeabi-v7a
APP_STL := gnustl_static
APP_CPPFLAGS := -std=c++11 -fsigned-char
APP_CPPFLAGS += -fexceptions -frtti -fpermissive