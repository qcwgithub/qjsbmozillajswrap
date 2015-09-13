编译 mozjswrap 库：

------------------------------------------------------------------------
Android (使用 NDK r9c)
------------------------------------------------------------------------

1. 打开 moz.gyp, 设置 js_debug 为 false. (line 9)

2. 重命名文件夹为 jni，重命名后布局是这样的：
jni/
    mozjswrap.cpp
    mozjswrap.h
    Android.mk

3. 命令行进入到 jni ，执行命令：
    sh gen-android.sh

4. 输出：../libs/armeabi-v7a/libmozjswrap.so



------------------------------------------------------------------------
Windows, Mac, iOS (使用 gyp, python, VisualStudio, XCode)
------------------------------------------------------------------------

1. 安装 python 2.7.x. (Windows版的在群文件里有)

2. 打开 moz.gyp, 如果是 iOS，设置 js_debug 设置为 false，Windows 和 Mac 设置为 true (line 9)

3. 编译
Windows: 执行 gen-msvs2012.bat      输出: ./build/Release/bin/mozjswrap.dll
iOS:     bash gen-ios.sh            输出: ./build/Release-iphoneos/libmozjswrap.a
Mac:     bash gen-mac.sh            输出: ./build/Default/mozjswrap.bundle
