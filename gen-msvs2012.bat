call python ./gyp/gyp_main.py -f msvs --depth=. ./moz.gyp -G msvs_version=2012 --generator-output=./build2012
devenv ./build2012/moz.sln  /Rebuild "Release|Win32" 
pause
