How to build the mozjswrap library:



------------------------------------------------------------------------
for Android (use NDK)
------------------------------------------------------------------------
1. rename the folder to jni, after rename, 
jni/
    mozjswrap.cpp
    mozjswrap.h
    Android.mk
2. inside the folder jni/, execute command
    sh gen-android.sh
3. output: ../libs/armeabi-v7a/libmozjswrap.so



------------------------------------------------------------------------
for Windows, Mac, iOS (use gyp, python, VisualStudio, XCode)
------------------------------------------------------------------------
1. Download google gyp. Put the gyp folder here. Like this:

mozjswrap.h
mozjswrap.cpp
gyp/
   gyp_main.py
   LICENSE

2. install python 2.7.x. (Mine is 2.7.6)
3. For iOS: open moz.gyp, change js_debug to false. (line 9)

4. build!
Windows: double click gen-msvs2012.bat      output: ./build/Release/bin/mozjswrap.dll
iOS:     bash gen-ios.sh                    output: ./build/Release-iphoneos/libmozjswrap.a
Mac:     bash gen-mac.sh                    output: ./build/Default/mozjswrap.bundle



Any questions? 
Email to answerwinner@gmail.com, 
or go to forum https://groups.google.com/forum/#!forum/js-binding-for-unity