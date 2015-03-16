#!/bin/bash
echo "generating IOS project files"

python ./gyp/gyp_main.py ./moz.gyp --depth=. -f xcode -DOS=ios --generator-output=.

xcodebuild -project moz.xcodeproj

