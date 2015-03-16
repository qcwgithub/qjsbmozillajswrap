call python ./gyp/gyp_main.py -f msvs --depth=. ./moz.gyp -G msvs_version=2012 --generator-output=./build
devenv ./build/moz.sln  /Rebuild "Release|Win32" 
pause
