#!/bin/bash
echo "generating Mac OS project files"

python ./gyp/gyp_main.py ./moz.gyp --depth=. -f xcode -DOS=mac --generator-output=.

xcodebuild -project moz.xcodeproj