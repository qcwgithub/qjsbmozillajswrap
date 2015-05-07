
#ifndef __MOZ_JSWRAP_HEADER__
#define __MOZ_JSWRAP_HEADER__

#ifdef ENABLE_JS_DEBUG
#pragma message("******** mozjswrap: js debug is enabled! ********") 
#else
#pragma message("******** mozjswrap: js debug is disabled! ********") 
#endif

#include "jsapi.h"
#include <vector>

#ifdef _WINDOWS
#if defined(MOZ_JSWRAP_COMPILATION)
#define MOZ_API __declspec( dllexport ) 
#else
#define MOZ_API __declspec( dllimport )
#endif
#else
#define MOZ_API
#endif

extern "C"{

	MOZ_API bool  JSh_Init(void);
	MOZ_API void JSh_ShutDown(void);
	MOZ_API JSRuntime* JSh_NewRuntime(uint32_t maxbytes, int useHelperThreads);
	MOZ_API void JSh_DestroyRuntime(JSRuntime *rt);
	MOZ_API void JSh_SetGCParameter(JSRuntime *rt, int key, uint32_t value);

	MOZ_API JSContext* JSh_NewContext(JSRuntime *rt, size_t stackChunkSize);
	MOZ_API void JSh_DestroyContext(JSContext *cx);
	MOZ_API void JSh_DestroyContextNoGC(JSContext *cx);

	MOZ_API JSErrorReporter JSh_SetErrorReporter(JSContext *cx, JSErrorReporter er);
	MOZ_API void* JSh_GetContextPrivate(JSContext *cx);
	MOZ_API void JSh_SetContextPrivate(JSContext *cx, void *data);
	MOZ_API void* JSh_GetPrivate(JSObject *obj);
	MOZ_API void JSh_SetPrivate(JSObject *obj, void *data);
	MOZ_API JSObject* JSh_GetParent(JSObject *obj);
	MOZ_API bool JSh_SetParent(JSContext *cx, JSObject *obj, JSObject *parent);

	MOZ_API JSObject* JSh_NewGlobalObject(JSContext *cx, int hookOption);

	MOZ_API bool JSh_InitStandardClasses(JSContext *cx, JSObject *obj);
	MOZ_API JSObject* JSh_InitReflect(JSContext *cx, JSObject *global);
	MOZ_API JSFunction* JSh_DefineFunction(JSContext *cx, JSObject *obj, const char *name, JSNative call, unsigned nargs, unsigned attrs);
    MOZ_API void JSh_ReportError(JSContext* cx, const char* sErr);
	MOZ_API int JSh_GetErroReportLineNo(JSErrorReport* report);
	MOZ_API const char* JSh_GetErroReportFileName(JSErrorReport* report);
	MOZ_API JSObject* JSh_NewArrayObject(JSContext *cx, int length, jsval *vector);
	MOZ_API bool JSh_IsArrayObject(JSContext *cx, JSObject *obj);
	// return -1: fail
	MOZ_API int JSh_GetArrayLength(JSContext *cx, JSObject *obj);
	MOZ_API bool JSh_GetElement(JSContext *cx, JSObject *obj, uint32_t index, jsval* val);
	MOZ_API bool JSh_SetElement(JSContext *cx, JSObject *obj, uint32_t index, jsval* pVal);
    MOZ_API bool JSh_GetProperty(JSContext *cx, JSObject *obj, const char* name, jsval* val);
	MOZ_API bool JSh_SetProperty(JSContext *cx, JSObject *obj, const char* name, jsval* pVal);
    MOZ_API bool JSh_GetUCProperty(JSContext *cx, JSObject *obj, jschar* name, int nameLen, jsval* val);
	MOZ_API bool JSh_SetUCProperty(JSContext *cx, JSObject *obj, jschar* name, int nameLen, jsval* pVal);
	// new a JSClass with specified flag and finalizer
	MOZ_API JSClass* JSh_NewClass(const char* name, unsigned int flag, JSFinalizeOp finalizeOp);

	// init a class with default value
	MOZ_API JSObject* JSh_InitClass(JSContext* cx, JSObject* glob, JSClass* jsClass);

	MOZ_API void JSh_GC(JSRuntime *rt);
	MOZ_API void JSh_MaybeGC(JSContext *cx);
	MOZ_API bool JSh_EvaluateScript(JSContext *cx, JSObject *obj,
		const char *bytes, unsigned length,
		const char *filename, unsigned lineno,
		jsval *rval);

	////////////////////////////////////////////////////////////////////////////////////
	// new a class and assign it a class

	MOZ_API JSObject* JSh_NewObjectAsClass(JSContext* cx, JSObject* glob, const char* className, JSFinalizeOp finalizeOp);

	MOZ_API JSObject* JSh_NewObject(JSContext *cx, JSClass *clasp, JSObject *proto, JSObject *parent);

	MOZ_API JSObject* JSh_NewMyClass(JSContext *cx, JSFinalizeOp finalizeOp);
    
    MOZ_API unsigned int JSh_ArgvTag(JSContext* cx, jsval* vp, int i);

	MOZ_API bool JSh_ArgvBool(JSContext* cx, jsval* vp, int i);
	MOZ_API double JSh_ArgvDouble(JSContext* cx, jsval* vp, int i);
	MOZ_API int JSh_ArgvInt(JSContext* cx, jsval* vp, int i);
	MOZ_API const jschar* JSh_ArgvString(JSContext* cx, jsval* vp, int i);
	MOZ_API const char* JSh_ArgvStringUTF8(JSContext* cx, jsval* vp, int i);
	MOZ_API JSObject* JSh_ArgvObject(JSContext* cx, jsval* vp, int i);
	MOZ_API JSFunction* JSh_ArgvFunction(JSContext* cx, jsval* vp, int i);

	MOZ_API bool JSh_ArgvFunctionValue(JSContext* cx, jsval* vp, int i, jsval* pval);
	////////////////////////////////////////////////////////////////////////////////////
	// returns

	MOZ_API void JSh_SetRvalBool(JSContext* cx, jsval* vp, bool value);
	MOZ_API void JSh_SetRvalDouble(JSContext* cx, jsval* vp, double value);
	MOZ_API void JSh_SetRvalInt(JSContext* cx, jsval* vp, int value);
	MOZ_API void JSh_SetRvalUInt(JSContext* cx, jsval* vp, unsigned int value);
	MOZ_API void JSh_SetRvalString(JSContext* cx, jsval* vp, const jschar* value);
	MOZ_API void JSh_SetRvalObject(JSContext* cx, jsval* vp, JSObject* value);
	MOZ_API void JSh_SetRvalUndefined(JSContext* cx, jsval* vp);
	MOZ_API void JSh_SetRvalJSVAL(JSContext* cx, jsval* vp, jsval* value);

	////////////////////////////////////////////////////////////////////////////////////
	// generate jsval

	MOZ_API void JSh_SetJsvalBool(jsval* vp, bool value);
	MOZ_API void JSh_SetJsvalDouble(jsval* vp, double value);
	MOZ_API void JSh_SetJsvalInt(jsval* vp, int value);
	MOZ_API void JSh_SetJsvalUInt(jsval* vp, unsigned int value);
	MOZ_API void JSh_SetJsvalString(JSContext* cx, jsval* vp, const jschar* value);
	MOZ_API void JSh_SetJsvalObject(jsval* vp, JSObject* value);
	MOZ_API void JSh_SetJsvalUndefined(jsval* vp);

	MOZ_API bool JSh_GetJsvalBool(jsval* vp);
	MOZ_API double JSh_GetJsvalDouble(jsval* vp);
	MOZ_API int JSh_GetJsvalInt(jsval* vp);
	MOZ_API unsigned int JSh_GetJsvalUInt(jsval* vp);
	MOZ_API const jschar* JSh_GetJsvalString(JSContext* cx, jsval* vp);
	MOZ_API JSObject* JSh_GetJsvalObject(jsval* vp);


	MOZ_API JSCompartment* JSh_EnterCompartment(JSContext *cx, JSObject *target);
	MOZ_API void JSh_LeaveCompartment(JSContext *cx, JSCompartment *oldCompartment);

	////////////////////////////////////////////////////////////////////////////////////

	MOZ_API void  JSh_SetTrustedPrincipals(JSRuntime *rt, const JSPrincipals *prin);
	MOZ_API JSScript* JSh_CompileScript(JSContext *cx, JSObject* global, const char *ascii, size_t length, const char* filename, size_t lineno);
	MOZ_API bool JSh_ExecuteScript(JSContext *cx, JSObject *obj, JSScript *script, jsval *rval);
	// this function is useless
	MOZ_API JSFunction* JSh_GetFunction(JSContext* cx, JSObject* obj, const char* name);
	MOZ_API bool JSh_GetFunctionValue(JSContext* cx, JSObject* obj, const char* name, jsval* val);
	MOZ_API bool JSh_ObjectIsFunction(JSContext *cx, JSObject *obj);

	MOZ_API bool JSh_CallFunction(JSContext *cx, JSObject *obj, JSFunction *fun, unsigned argc, jsval *argv, jsval *rval);
	MOZ_API bool JSh_CallFunctionName(JSContext *cx, JSObject *obj, const char *name, unsigned argc, jsval *argv, jsval *rval);
	MOZ_API bool JSh_CallFunctionValue(JSContext *cx, JSObject *obj, jsval* fval, unsigned argc, jsval *argv, jsval *rval);

	MOZ_API bool JSh_IsNative(JSObject *obj);
	MOZ_API JSRuntime* JSh_GetObjectRuntime(JSObject *obj);
	MOZ_API JSObject* JSh_NewObjectWithGivenProto(JSContext *cx, const JSClass *clasp, JSObject *proto, JSObject *parent);
	MOZ_API bool JSh_DeepFreezeObject(JSContext *cx, JSObject *obj);
	MOZ_API bool JSh_FreezeObject(JSContext *cx, JSObject *obj);

	MOZ_API bool JSh_StringHasBeenInterned(JSContext *cx, JSString *str);

	//     MOZ_API jsid INTERNED_STRING_TO_JSID(JSContext *cx, JSString *str)
	//     {
	//         return INTERNED_STRING_TO_JSID(cx, str);
	//     }

	// this function compile fail on mac, don't know why
	//     MOZ_API bool JSh_CallOnce(JSCallOnceType *once, JSInitCallback func)
	//     {
	//         return JS_CallOnce(once, func);
	//     }
	MOZ_API int64_t JSh_Now(void);
	MOZ_API jsval JSh_GetNaNValue(JSContext *cx);

	MOZ_API jsval JSh_GetNegativeInfinityValue(JSContext *cx);

	MOZ_API jsval JSh_GetPositiveInfinityValue(JSContext *cx);

	MOZ_API jsval JSh_GetEmptyStringValue(JSContext *cx);

	MOZ_API JSString* JSh_GetEmptyString(JSRuntime *rt);

	//     MOZ_API bool JS_ConvertValue(JSContext *cx, JS::HandleValue v, JSType type, JS::MutableHandleValue vp);
	//     MOZ_API bool JS_ValueToObject(JSContext *cx, JS::HandleValue v, JS::MutableHandleObject objp);
	MOZ_API JSFunction* JSh_ValueToFunction(JSContext *cx, jsval* v);
	MOZ_API JSFunction* JSh_ValueToConstructor(JSContext *cx, jsval* v);

	MOZ_API JSString* JSh_ValueToSource(JSContext *cx, jsval v);
	MOZ_API bool JSh_DoubleIsInt32(double d, int32_t *ip);
	MOZ_API int32_t JSh_DoubleToInt32(double d);

	MOZ_API uint32_t JSh_DoubleToUint32(double d);

	MOZ_API JSType JSh_TypeOfValue(JSContext *cx, jsval v);
	MOZ_API const char * JSh_GetTypeName(JSContext *cx, JSType type);
	MOZ_API bool JSh_StrictlyEqual(JSContext *cx, jsval v1, jsval v2, bool *equal);
	MOZ_API bool JSh_LooselyEqual(JSContext *cx, jsval v1, jsval v2, bool *equal);
	MOZ_API bool JSh_SameValue(JSContext *cx, jsval v1, jsval v2, bool *same);

	MOZ_API bool JSh_IsBuiltinEvalFunction(JSFunction *fun);
	MOZ_API bool JSh_IsBuiltinFunctionConstructor(JSFunction *fun);
	MOZ_API void* JSh_GetRuntimePrivate(JSRuntime *rt);
	MOZ_API void JSh_SetRuntimePrivate(JSRuntime *rt, void *data);
	MOZ_API void JSh_BeginRequest(JSContext *cx);
	MOZ_API void JSh_EndRequest(JSContext *cx);
	MOZ_API bool JSh_IsInRequest(JSRuntime *rt);
	MOZ_API void JSh_SetJitHardening(JSRuntime *rt, bool enabled);
	MOZ_API const char * JSh_GetImplementationVersion(void);
	MOZ_API void JSh_SetDestroyCompartmentCallback(JSRuntime *rt, JSDestroyCompartmentCallback callback);
	MOZ_API void JSh_SetDestroyZoneCallback(JSRuntime *rt, JSZoneCallback callback);
	MOZ_API void JSh_SetSweepZoneCallback(JSRuntime *rt, JSZoneCallback callback);
	MOZ_API void JSh_SetCompartmentNameCallback(JSRuntime *rt, JSCompartmentNameCallback callback);
	MOZ_API void JSh_SetWrapObjectCallbacks(JSRuntime *rt, const JSWrapObjectCallbacks *callbacks);
	MOZ_API void JSh_SetCompartmentPrivate(JSCompartment *compartment, void *data);
	MOZ_API void* JSh_GetCompartmentPrivate(JSCompartment *compartment);
	MOZ_API void JSh_SetZoneUserData(JS::Zone *zone, void *data);
	MOZ_API void* JSh_GetZoneUserData(JS::Zone *zone);

	//     MOZ_API bool JS_WrapObject(JSContext *cx, JS::MutableHandleObject objp);
	//     MOZ_API bool JS_WrapValue(JSContext *cx, JS::MutableHandleValue vp);
	//     MOZ_API bool JS_WrapId(JSContext *cx, jsid *idp);
	//     MOZ_API JSObject* JS_TransplantObject(JSContext *cx, JS::Handle<JSObject*> origobj, JS::Handle<JSObject*> target);
	//     MOZ_API bool JS_RefreshCrossCompartmentWrappers(JSContext *cx, JSObject *ob);
	//     MOZ_API void JS_IterateCompartments(JSRuntime *rt, void *data, JSIterateCompartmentCallback compartmentCallback);
	// 
	// 
	//     MOZ_API bool JS_ResolveStandardClass(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> id, bool *resolved);
	//     MOZ_API bool JS_EnumerateStandardClasses(JSContext *cx, JS::Handle<JSObject*> obj);
	//     MOZ_API bool JS_GetClassObject(JSContext *cx, JSObject *obj, JSProtoKey key, JSObject **objp);
	//     MOZ_API bool JS_GetClassPrototype(JSContext *cx, JSProtoKey key, JSObject **objp);

	MOZ_API JSProtoKey JSh_IdentifyClassPrototype(JSContext *cx, JSObject *obj);

	MOZ_API JSObject* JSh_ThisObject(JSContext* cx, jsval* vp);
	MOZ_API bool JSh_AddValueRoot(JSContext *cx, jsval *vp);
	MOZ_API bool JSh_AddStringRoot(JSContext *cx, JSString **rp);
	MOZ_API bool JSh_AddObjectRoot(JSContext *cx, JSObject **rp);
	MOZ_API bool JSh_AddNamedValueRoot(JSContext *cx, jsval *vp, const char *name);
	MOZ_API bool JSh_AddNamedValueRootRT(JSRuntime *rt, jsval *vp, const char *name);
	MOZ_API bool JSh_AddNamedStringRoot(JSContext *cx, JSString **rp, const char *name);
	MOZ_API bool JSh_AddNamedObjectRoot(JSContext *cx, JSObject **rp, const char *name);
	MOZ_API bool JSh_AddNamedScriptRoot(JSContext *cx, JSScript **rp, const char *name);
	MOZ_API void JSh_RemoveValueRoot(JSContext *cx, jsval *vp);
	MOZ_API void JSh_RemoveStringRoot(JSContext *cx, JSString **rp);
	MOZ_API void JSh_RemoveObjectRoot(JSContext *cx, JSObject **rp);
	MOZ_API void JSh_RemoveScriptRoot(JSContext *cx, JSScript **rp);
	MOZ_API void JSh_RemoveValueRootRT(JSRuntime *rt, jsval *vp);
	MOZ_API void JSh_RemoveStringRootRT(JSRuntime *rt, JSString **rp);
	MOZ_API void JSh_RemoveObjectRootRT(JSRuntime *rt, JSObject **rp);
	MOZ_API void JSh_RemoveScriptRootRT(JSRuntime *rt, JSScript **rp);
	MOZ_API  void JSh_SetNativeStackQuota(JSRuntime *cx, size_t systemCodeStackSize, size_t trustedScriptStackSize, size_t untrustedScriptStackSize);

	////////////////////////////////////////////////////////////////////////////////////
	// debugger api
	MOZ_API void JSh_EnableDebugger(JSContext* cx, JSObject* global, const char** src_searchpath, int nums, int port);
	MOZ_API void JSh_UpdateDebugger();
    MOZ_API void JSh_CleanupDebugger();
    // some useful api
    MOZ_API bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file);
    MOZ_API void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file);
    MOZ_API void JSh_DumpBacktrace(JSContext* cx);


    //
    // some useful function
    //
    typedef void* PPV;
    MOZ_API PPV* JSh_NewPPointer(PPV pValue);
    MOZ_API void JSh_DelPPointer(PPV* p);
    MOZ_API void JSh_SetPPointer(PPV* p, PPV pValue);
    MOZ_API PPV JSh_GetPPointer(PPV* p);

    struct MyHeapObj
    {
        JS::Heap<JSObject*>* heapJSObj;
        JSObject* jsObj;

        JS::Heap<JSObject*>* heapNativeObj;
        JSObject* nativeObj;
    };

    // MOZ_API void 

    MOZ_API JS::Heap<JSObject*>* JSh_NewHeapObject(JSObject* obj);
    MOZ_API void JSh_DelHeapObject(JS::Heap<JSObject*>* heapObj);

    MOZ_API void JSh_SetGCCallback(JSRuntime *rt, JSGCCallback cb, void *data);

    MOZ_API void InitPersistentObject(JSRuntime* rt, JSContext* cx, JSObject* global, JSFinalizeOp finalizer);
    MOZ_API bool NewJSClassObject(char* name, JSObject** retJSObj, JSObject** retNativeObj);
    MOZ_API bool NewJSClassObjectRef(char* name, JSObject** retJSObj, JSObject** retNativeObj, JSObject* objWrap);
}

#endif // #ifndef __MOZ_JSWRAP_HEADER__