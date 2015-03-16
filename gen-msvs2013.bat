cd gyp
call python ../../tools/gyp/gyp_main.py -f msvs --depth=. moz.gyp -G msvs_version=2013 --generator-output=../build
cd ..
devenv ./build/moz.sln  /Rebuild "Release|Win32" 
pause