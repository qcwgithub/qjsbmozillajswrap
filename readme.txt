How to build the mozjswrap library:

------------------------------------------------------------------------
for Android (use NDK r9c)
------------------------------------------------------------------------
1. Open moz.gyp, change js_debug to false. (line 9)
2. rename the folder to jni, after rename, 
jni/
    mozjswrap.cpp
    mozjswrap.h
    Android.mk
3. inside the folder jni/, execute command
    sh gen-android.sh
4. output: ../libs/armeabi-v7a/libmozjswrap.so



------------------------------------------------------------------------
for Windows, Mac, iOS (use gyp, python, VisualStudio, XCode)
------------------------------------------------------------------------
1. install python 2.7.x. (Mine is 2.7.6)
2. For iOS: open moz.gyp, change js_debug to false. (line 9)

3. build!
Windows: double click gen-msvs2012.bat      output: ./build/Release/bin/mozjswrap.dll
iOS:     bash gen-ios.sh                    output: ./build/Release-iphoneos/libmozjswrap.a
Mac:     bash gen-mac.sh                    output: ./build/Default/mozjswrap.bundle
