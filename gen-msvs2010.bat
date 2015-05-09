call python ./gyp/gyp_main.py -f msvs --depth=. ./moz.gyp -G msvs_version=2010 --generator-output=./build
devenv ./build/moz.sln  /Rebuild "Release|Win32" 
pause
