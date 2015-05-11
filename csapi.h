#ifndef __cs_api_h__
#define __cs_api_h__

#include "mozjswrap.h"

extern "C"
{

MOZ_API int InitJSEngine(JSErrorReporter er);
MOZ_API void ShutdownJSEngine();
MOZ_API bool NewJSClassObject(char* name, JSObject** retJSObj, JSObject** retNativeObj, JSObject* objRef);
MOZ_API JSContext* GetContext();
MOZ_API JSObject* GetGlobal();
MOZ_API JSRuntime* GetRuntime();

}
#endif // #ifndef __cs_api_h__