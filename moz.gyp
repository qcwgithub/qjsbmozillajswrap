{
    'targets': [
        {
            'target_name': 'mozjswrap',
            'type': 'shared_library',
            'variables': 
            {
                'source_path': '.',
                'js_debug': 'true', #enable js debug? true or false (you can change this)
            },
            'include_dirs': 
            [
                '<(source_path)/debugger',
                '<(source_path)/debugger/cocos',
            ],            
            'includes': [ 'common.gypi', ],
            'defines': ['MOZ_JSWRAP_COMPILATION'],
            'sources': [
                '<(source_path)/mozjswrap.cpp',
                '<(source_path)/mozjswrap.h'
            ],
            'conditions': 
            [
                    
                # if debug is enabled
                # add debugger files to project
                ['js_debug == "true"', 
                {
                    'sources+':  [
                        '<(source_path)/debugger/jsdebugger.cpp',
                        '<(source_path)/debugger/jsdebugger.h',
                        '<(source_path)/debugger/global_define.h',
                        '<(source_path)/debugger/cocos/CCData.cpp',
                        '<(source_path)/debugger/cocos/CCData.h',
                        '<(source_path)/debugger/cocos/CCFileUtils.cpp',
                        '<(source_path)/debugger/cocos/CCFileUtils.h',
                        '<(source_path)/debugger/cocos/CCValue.cpp',
                        '<(source_path)/debugger/cocos/CCValue.h',
                        '<(source_path)/debugger/cocos/ConvertUTF.c',
                        '<(source_path)/debugger/cocos/ConvertUTF.h',
                        '<(source_path)/debugger/cocos/ConvertUTFWrapper.cpp',
                        '<(source_path)/debugger/cocos/js_manual_conversions.cpp',
                        '<(source_path)/debugger/cocos/js_manual_conversions.h',
                        '<(source_path)/debugger/cocos/jsb_fileutils.cpp',
                        '<(source_path)/debugger/cocos/jsb_fileutils.h',
                        '<(source_path)/debugger/cocos/spidermonkey_specifics.h',
                        '<(source_path)/debugger/cocos/uthash.h',
                     ],

                    'defines+':  ['ENABLE_JS_DEBUG'],
                },],

            #### windows start ####

                ['OS == "win"', 
                {
                    'include_dirs+': ['<(source_path)/spidermonkey/include/win32','<(source_path)/debugger/cocos/win32'],
                    'sources+': ['<(source_path)/dllmain.cpp',],
                    'conditions': 
                    [
                        ['js_debug == "true"', {
                            'sources+':  ['<(source_path)/debugger/cocos/win32/CCFileUtils-win32.cpp',
                                          '<(source_path)/debugger/cocos/win32/CCFileUtils-win32.h', ],
                        },],
                    ],
                    'msbuild_settings': {
                            'Link': {
                                'AdditionalDependencies': [
                                    '../spidermonkey/prebuilt/win32/mozjs-28.lib',
                                ],
                            },
                    },
                    'copies': [
                        {
                          'destination': '<(PRODUCT_DIR)bin',
                          'files': [
                            '<(source_path)/spidermonkey/prebuilt/win32/mozjs-28.lib',
                            '<(source_path)/spidermonkey/prebuilt/win32/mozjs-28.dll',
                          ],
                        },
                    ],
                },],

            #### android start ####

                ['OS == "android"',
                {
                    'include_dirs+':['<(source_path)/spidermonkey/include/android','<(source_path)/debugger/cocos/android',],
                    'conditions': 
                    [
                        [
                            'js_debug == "true"', {
                                'sources+':  [
                                    '<(source_path)/debugger/cocos/android/CCFileUtils-android.cpp',
                                    '<(source_path)/debugger/cocos/android/CCFileUtils-android.h',
                                    '<(source_path)/debugger/cocos/android/Java_org_cocos2dx_lib_Cocos2dxHelper.cpp',
                                    '<(source_path)/debugger/cocos/android/Java_org_cocos2dx_lib_Cocos2dxHelper.h',
                                    '<(source_path)/debugger/cocos/android/JniHelper.cpp',
                                    '<(source_path)/debugger/cocos/android/JniHelper.h',
                                ],
                            },
                        ],
                    ],
                    'link_settings': {
                        'ldflags': [
                            '-lz',
                            '-llog',
                            '-ldl',
                            '-lGLESv2',
                            '-lEGL',
                            '-landroid',
                        ],
                        'cflags_cc':[
                            '-D__STDC_LIMIT_MACROS=1',
                            '-Wno-invalid-offsetof',
                        ],
                        'libraries':['<(source_path)/spidermonkey/prebuilt/android/armeabi-v7a/libjs_static.a'],
                    },
                },],

            #### iOS start ####

                ['OS == "ios"',
                {
                    'type': 'static_library',
                    'include_dirs+':['<(source_path)/spidermonkey/include/ios','<(source_path)/debugger/cocos/apple'],
                    'conditions': 
                    [
                        ['js_debug == "true"', {
                            'sources+':  ['<(source_path)/debugger/cocos/apple/CCFileUtils-apple.mm',
                                          '<(source_path)/debugger/cocos/apple/CCFileUtils-apple.h',],
                        },],
                    ],
                    #'xcode_config_file': '<(source_path)/debugger/cocos/apple/mozjs.xcconfig',
                },],

            #### Mac start ####

                ['OS == "mac"',
                {
                    #'default_configuration': 'Release',
                    'include_dirs+':['<(source_path)/spidermonkey/include/mac','<(source_path)/debugger/cocos/apple'],

                    # loadable_module+bundle makes a bundle
                    'type': 'loadable_module', #static_library, shared_library, executable
                    'mac_bundle': 1,
                    #'is_xctest':1,

                    'xcode_settings':
                    {
                        "GCC_OPTIMIZATION_LEVEL":"3",
                        "GCC_GENERATE_DEBUGGING_SYMBOLS":"NO",
                        "DEAD_CODE_STRIPPING":"YES",
                        "GCC_INLINES_ARE_PRIVATE_EXTERN":"YES",

                        'ARCHS': '$(ARCHS_STANDARD_32_BIT)',
                        'CLANG_CXX_LANGUAGE_STANDARD':'c++0x',

                        #'OTHER_CFLAGS':['-fobjc-arc'],
                        #'SDKROOT': 'macosx10.9',
                    },

                    'conditions': 
                    [
                        ['js_debug == "true"', {
                            'sources+':  ['<(source_path)/debugger/cocos/apple/CCFileUtils-apple.mm',
                                          '<(source_path)/debugger/cocos/apple/CCFileUtils-apple.h',],
                        },],
                    ],
                    #'xcode_config_file': '<(source_path)/debugger/cocos/apple/mozjs.xcconfig',
                    'libraries+':['<(source_path)/spidermonkey/prebuilt/mac/i386/libjs_static.a', 'libz.dylib'],
               },],


            ], # conditions in first target

        }, # first target
    ], # targets
}
