call python ./gyp/gyp_main.py -f msvs --depth=. ./moz.gyp -G msvs_version=2013 --generator-output=./build2013
devenv ./build2013/moz.sln  /Rebuild "Release|Win32" 
pause
