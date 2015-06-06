# 各个平台的设置
#
{
    'conditions' : [
        [ 'OS == "win"',
            {
                'msvs_disabled_warnings': [
                    4204,        # 警告：用了非标准扩展 : 非常量聚合初始值设定项
                    4100,        # 警告：未引用的形参
                    4152,        # 警告：表达式中的函数/数据指针转换
                    4200,        # 警告：使用了非标准扩展 : 结构/联合中的零大小数组
                    4505,        # 警告：未引用的本地函数已移除(局部函数定义了，但未使用)
                    4800,        # 警告：将值强制为布尔值“true”或“false”(性能警告)
                    4201,        # 警告：使用了非标准扩展 : 无名称的结构/联合
                    4189,   # local veriable is initialized but not referenced
                    4127,        # conditional expression is constant
                    4512,   # assignment operator could not be generated
                    4099,        # type name first seen using 'class' now seen using 'struct'
                    4251,   # 
                ], 
                'msbuild_configuration_attributes': {
                        'CharacterSet': '2',                                                # 1: unicode字符集 2: multi bytes
                        'OutputDirectory': '$(SolutionDir)\\$(Configuration)',                      # $(OutDir)
                          'IntermediateDirectory': '$(OutDir)\\obj\\$(ProjectName)', # $(IntDir)
                },
                #'cflags':['/WX-',], #禁止 warning treated as error - no 'object' file generated
                'msbuild_settings': {
                        'ClCompile': {
                            'TreatWChar_tAsBuiltInType': 'true',                              #将WChar_t视为内置类型
                            'TreatWarningAsError': 'false',                                  #将警告视为错误
                            'WarningLevel': 'Level4',                                        #警告等级
                            'ProgramDataBaseFileName': '$(IntDir)vc$(PlatformToolsetVersion).pdb',                            
                            'PreprocessorDefinitions': ['WIN32','_USING_V110_SDK71_','_WINDOWS'],     #预处理器定义
                            #'ExceptionHandling': '/EHsc',
                        },
                        'Link': {
                            'MinimumRequiredVersion':'5.01',                                #用来支持XP
                            'SubSystem': 'Console',                                            #控制台
                            #'LinkIncremental': 'true',
                            'ProgramDatabaseFile': '$(OutDir)pdb\\$(TargetName).pdb', #生成程序数据库文件
                            #'GenerateManifest': 'false',
                            'GenerateDebugInformation': 'false',                              #生成调试信息
                            'ImportLibrary' : '$(OutDir)bin\\$(TargetName).lib',         #导入库存放位置
                            'OutputFile': '$(OutDir)bin\\$(TargetName)$(TargetExt)',    #输出文件
                            'ImageHasSafeExceptionHandlers': 'false',                        #映像具有安全异常处理程序
                            'AdditionalDependencies': [                                            #附加依赖项
                                'kernel32.lib',
                                'gdi32.lib',
                                'winspool.lib',
                                'comdlg32.lib',
                                'advapi32.lib',
                                'shell32.lib',
                                'ole32.lib',
                                'oleaut32.lib',
                                'user32.lib',
                                'uuid.lib',
                                'odbc32.lib',
                                'odbccp32.lib',
                                'DelayImp.lib',
                                'winmm.lib',
                                'ws2_32.lib',
                            ],
                        },
                },
                'configurations': {
                    'Debug': {
                        'msbuild_settings': {
                            'ClCompile': {
                                'BasicRuntimeChecks': 'EnableFastChecks',                    #基本运行时检查（/RTC1）
                                'DebugInformationFormat': 'EditAndContinue',                 # editAndContiue (/ZI)
                                'Optimization': 'Disabled',                                   # optimizeDisabled (/Od)
                                'PreprocessorDefinitions': ['_DEBUG'],                        #预处理器定义
                                'RuntimeLibrary': 'MultiThreadedDebugDLL',                     # rtMultiThreadedDebug (/MTd)
                                #'RuntimeTypeInfo': 'false',                                  # /GR-
                                'IntrinsicFunctions': 'false'                                #启用内部函数（否）
                            },
                            'Link': {
                                'GenerateDebugInformation': 'true',                         #生成调试信息
                            },
                        },
                        'msbuild_configuration_attributes': {
                            'LinkIncremental': 'true',
                            'GenerateManifest': 'false',
                        },
                    },
                    'Release': {
                        'msbuild_settings': {
                            'ClCompile': {
                                'DebugInformationFormat': 'ProgramDatabase',                  # programDatabase (/Zi)
                                'Optimization': 'MaxSpeed',                                    # optimizeDisabled (/O2)
                                'WholeProgramOptimization': 'true',                         #/GL
                                 'PreprocessorDefinitions': ['NDEBUG'],
                                'RuntimeLibrary': 'MultiThreadedDLL',                          # rtMultiThreaded (/MT)
                                #'RuntimeTypeInfo': 'false',                                 # /GR-
                                'IntrinsicFunctions': 'true'                                #启用内部函数（是）
                            },
                            'Link': {
                                #'LinkIncremental': 'false',
                                #'GenerateManifest': 'true',
                                'GenerateDebugInformation': 'true',                            #生成调试信息
                            },
                        },
                        'msbuild_configuration_attributes': {
                            'LinkIncremental': 'false',                                        #不启用增量链接
                            'GenerateManifest': 'false',                                    #不生成manifest
                        },
                    },
                },
            },
        ],
        ['OS == "android"',{
            'cflags': [
              '-Werror',
            ],
          },
        ],
        ['OS == "ios"',{
            'default_configuration':"Release",
            'xcode_settings': {
                        'SDKROOT': 'iphoneos',
                        'TARGETED_DEVICE_FAMILY': '1,2',
                        'CODE_SIGN_IDENTITY': 'iPhone Developer',
                        'IPHONEOS_DEPLOYMENT_TARGET': '6.0',
                        'ARCHS': '$(ARCHS_STANDARD)',
                        'CLANG_CXX_LANGUAGE_STANDARD':'c++0x',
                },
            'configurations':{

                'Debug':{
                    'defines':[ 'DEBUG'],
                    'xcode_settiings':{
                        "GCC_OPTIMIZATION_LEVEL":"0",
                        "GCC_GENERATE_DEBUGGING_SYMBOLS":"YES",
                    }
                },
                'Release':{
                    'defines':[ 'NDEBUG'],
                    'xcode_settiings':{
                        "GCC_OPTIMIZATION_LEVEL":"3",
                        "GCC_GENERATE_DEBUGGING_SYMBOLS":"NO",
                        "DEAD_CODE_STRIPPING":"YES",
                        "GCC_INLINES_ARE_PRIVATE_EXTERN":"YES",
                    },
                },
            },
          },
        ],
    ]    
}
