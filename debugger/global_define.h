
#pragma once


#define PLATFORM_UNKNOWN            0
#define PLATFORM_IOS                1
#define PLATFORM_ANDROID            2
#define PLATFORM_WIN32              3
#define PLATFORM_MARMALADE          4
#define PLATFORM_LINUX              5
#define PLATFORM_BADA               6
#define PLATFORM_BLACKBERRY         7
#define PLATFORM_MAC                8
#define PLATFORM_NACL               9
#define PLATFORM_EMSCRIPTEN        10
#define PLATFORM_TIZEN             11
#define PLATFORM_QT5               12
#define PLATFORM_WP8               13
#define PLATFORM_WINRT             14


// Determine target platform by compile environment macro.
#define TARGET_PLATFORM             PLATFORM_UNKNOWN


// win32
#if defined(_WIN32) && defined(_WINDOWS)
#undef  TARGET_PLATFORM
#define TARGET_PLATFORM         PLATFORM_WIN32
#endif

// android
#if defined(ANDROID)
#undef  TARGET_PLATFORM
#define TARGET_PLATFORM         PLATFORM_ANDROID
#include "jsdebugger.h"
#endif



// mac
#if defined(CC_TARGET_OS_MAC) || defined(__APPLE__)
#undef  TARGET_PLATFORM
#define TARGET_PLATFORM         PLATFORM_MAC
#include "jsdebugger.h"
#endif


 
// iphone
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#undef  TARGET_PLATFORM
#define TARGET_PLATFORM         PLATFORM_IOS
#include "jsdebugger.h"

#endif


#define CC_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define CC_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define CC_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define CC_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define CC_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define CC_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define CC_BREAK_IF(cond)           if(cond) break


#if COCOS2D_DEBUG
#define TRACE_DEBUGGER_SERVER(...) CCLOG(__VA_ARGS__)
#else
#define TRACE_DEBUGGER_SERVER(...)
#endif // #if DEBUG

#ifndef CCASSERT
#if COCOS2D_DEBUG > 0
#if CC_ENABLE_SCRIPT_BINDING
extern bool CC_DLL cc_assert_script_compatible(const char *msg);
#define CCASSERT(cond, msg) do {                              \
          if (!(cond)) {                                          \
            if (!cc_assert_script_compatible(msg) && strlen(msg)) \
              cocos2d::log("Assert failed: %s", msg);             \
            CC_ASSERT(cond);                                      \
		  		  		  		            } \
        } while (0)
#else
#define CCASSERT(cond, msg) CC_ASSERT(cond)
#endif
#else
#define CCASSERT(cond, msg)
#endif

#define GP_ASSERT(cond) CCASSERT(cond, "")

// FIXME:: Backward compatible
#define CCAssert CCASSERT
#endif  // CCASSERT

// cocos2d debug
#if !defined(COCOS2D_DEBUG) || COCOS2D_DEBUG == 0
#define CCLOG(...)       do {} while (0)
#define CCLOGINFO(...)   do {} while (0)
#define CCLOGERROR(...)  do {} while (0)
#define CCLOGWARN(...)   do {} while (0)

#elif COCOS2D_DEBUG == 1
#define CCLOG(format, ...)      cocos2d::log(format, ##__VA_ARGS__)
#define CCLOGERROR(format,...)  cocos2d::log(format, ##__VA_ARGS__)
#define CCLOGINFO(format,...)   do {} while (0)
#define CCLOGWARN(...) __CCLOGWITHFUNCTION(__VA_ARGS__)

#elif COCOS2D_DEBUG > 1
#define CCLOG(format, ...)      cocos2d::log(format, ##__VA_ARGS__)
#define CCLOGERROR(format,...)  cocos2d::log(format, ##__VA_ARGS__)
#define CCLOGINFO(format,...)   cocos2d::log(format, ##__VA_ARGS__)
#define CCLOGWARN(...) __CCLOGWITHFUNCTION(__VA_ARGS__)
#endif // COCOS2D_DEBUG


/** @def JSB_ASSERT_ON_FAIL
Whether or not to assert when the arguments or conversions are incorrect.
It is recommened to turn it off in Release mode.
*/
#ifndef JSB_ASSERT_ON_FAIL
#define JSB_ASSERT_ON_FAIL 0
#endif


#if JSB_ASSERT_ON_FAIL
#define JSB_PRECONDITION( condition, error_msg) do { NSCAssert( condition, [NSString stringWithUTF8String:error_msg] ); } while(0)
#define JSB_PRECONDITION2( condition, context, ret_value, error_msg) do { NSCAssert( condition, [NSString stringWithUTF8String:error_msg] ); } while(0)
#define ASSERT( condition, error_msg) do { NSCAssert( condition, [NSString stringWithUTF8String:error_msg] ); } while(0)

#else
#define JSB_PRECONDITION( condition, ...) do {                          \
    if( ! (condition) ) {                                                       \
        CCLOG("jsb: ERROR: File %s: Line: %d, Function: %s", __FILE__, __LINE__, __FUNCTION__ );         \
        CCLOG(__VA_ARGS__);                                        \
        JSContext* globalContext = ScriptingCore::getInstance()->getGlobalContext();    \
        if( ! JS_IsExceptionPending( globalContext ) ) {                        \
            JS_ReportError( globalContext, __VA_ARGS__ );                           \
		        }                                                                       \
        return false;                                                       \
	    }                                                                           \
} while(0)
#define JSB_PRECONDITION2( condition, context, ret_value, ...) do {             \
    if( ! (condition) ) {                                                       \
        CCLOG("jsb: ERROR: File %s: Line: %d, Function: %s", __FILE__, __LINE__, __FUNCTION__ );         \
        CCLOG(__VA_ARGS__);                                        \
        if( ! JS_IsExceptionPending( context ) ) {                          \
            JS_ReportError( context, __VA_ARGS__ );                             \
		        }                                                                       \
        return ret_value;                                                       \
	    }                                                                           \
} while(0)
#define ASSERT( condition, error_msg) do {                                      \
    if( ! (condition) ) {                                                       \
        CCLOG("jsb: ERROR in %s: %s\n", __FUNCTION__, error_msg);               \
        return false;                                                           \
	    }                                                                           \
	    } while(0)
#endif

#define JSB_PRECONDITION3( condition, context, ret_value, ...) do { \
        if( ! (condition) ) return (ret_value); \
			    } while(0)

#define JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET \
JSAutoCompartment __jsb_ac(jsdebugger::getInstance()->getGlobalContext(), jsdebugger::getInstance()->getGlobalObject());

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CC_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#if __has_attribute(format)
#define CC_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#endif // __has_attribute(format)
#else
#define CC_FORMAT_PRINTF(formatPos, argPos)
#endif

void log(const char* format,...) CC_FORMAT_PRINTF(1,2);

