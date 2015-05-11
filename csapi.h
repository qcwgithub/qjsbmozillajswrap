#ifndef __cs_api_h__
#define __cs_api_h__

#include "mozjswrap.h"

extern "C"
{

MOZ_API int InitJSEngine(JSErrorReporter er, CSEntry entry, JSNative req);
MOZ_API void ShutdownJSEngine();
MOZ_API OBJID NewJSClassObject(char* name);
MOZ_API bool RemoveJSClassObject(OBJID odjID);
MOZ_API bool IsJSClassObjectFunctionExist(OBJID objID, const char* functionName);
MOZ_API JSContext* GetContext();
MOZ_API JSObject* GetGlobal();
MOZ_API JSRuntime* GetRuntime();

}
#endif // #ifndef __cs_api_h__