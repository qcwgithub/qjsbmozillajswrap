// mozjswrap.cpp
// author: answerwinner
// desc: wraps for SpiderMonkey api

#include "mozjswrap.h"

#ifdef ENABLE_JS_DEBUG
#include "debugger/jsdebugger.h"
#include "global_define.h"
#include "js/OldDebugAPI.h"
#endif

//
// memory for marshaling
//
void* marshalMemory = 0;
int marshalLength = 0;
void* getMarshalMemory(int len)
{
    if (marshalMemory == 0 || marshalLength < len) 
    {
        if (marshalMemory)
            free(marshalMemory);
        marshalMemory = malloc(len);
        marshalLength = len;
    }
    return marshalMemory;
}

jschar* getMarshalStringFromJSString(JSContext* cx, JSString* jsStr)
{
    size_t length = 0;
    const jschar* ori = JS_GetStringCharsAndLength(cx, jsStr, &length);
    jschar* rt = (jschar*)::getMarshalMemory(sizeof(jschar) * (length + 1));
    rt[length] = 0;
    memcpy(rt, ori, length * sizeof(jschar));
    return rt;
}

JSRuntime* g_rt = 0;
JSContext* g_cx = 0;
JSObject* g_global = 0;
JSFinalizeOp g_finalizer = 0;

void sc_finalize(JSFreeOp* freeOp, JSObject* obj)
{

}

static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sc_finalize,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSClass qiucw_class =
{
	"qiucw", 0,
	JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

bool JSh_Init(void)
{
	return JS_Init();
}
void JSh_ShutDown(void)
{
	JS_ShutDown();
}
JSRuntime* JSh_NewRuntime(uint32_t maxbytes, int useHelperThreads)
{
	return JS_NewRuntime(maxbytes, (JSUseHelperThreads)useHelperThreads);
}
void JSh_DestroyRuntime(JSRuntime *rt)
{
	JS_DestroyRuntime(rt);
}
void JSh_SetGCParameter(JSRuntime *rt, int key, uint32_t value)
{
	return JS_SetGCParameter(rt, (JSGCParamKey)key, value);
}

JSContext* JSh_NewContext(JSRuntime *rt, size_t stackChunkSize)
{
	return JS_NewContext(rt, stackChunkSize);
}
void JSh_DestroyContext(JSContext *cx)
{
	JS_DestroyContext(cx);
}
void JSh_DestroyContextNoGC(JSContext *cx)
{
	JS_DestroyContextNoGC(cx);
}

JSErrorReporter JSh_SetErrorReporter(JSContext *cx, JSErrorReporter er)
{
	return JS_SetErrorReporter(cx, er);
}
void* JSh_GetContextPrivate(JSContext *cx)
{
	return JS_GetContextPrivate(cx);
}
void JSh_SetContextPrivate(JSContext *cx, void *data)
{
	JS_SetContextPrivate(cx, data);
}
void* JSh_GetPrivate(JSObject *obj)
{
	return JS_GetPrivate(obj);
}
void JSh_SetPrivate(JSObject *obj, void *data)
{
	JS_SetPrivate(obj, data);
}
JSObject* JSh_GetParent(JSObject *obj)
{
	return JS_GetParent(obj);
}
bool JSh_SetParent(JSContext *cx, JSObject *obj, JSObject *parent)
{
	return JS_SetParent(cx, obj, parent);
}

JSObject* JSh_NewGlobalObject(JSContext *cx, int hookOption)
{
	JS::CompartmentOptions options;
	options.setVersion(JSVERSION_LATEST);

	return JS_NewGlobalObject(cx, &global_class, 0/*principals*/, (JS::OnNewGlobalHookOption)hookOption, options);
}

bool JSh_InitStandardClasses(JSContext *cx, JSObject *obj)
{
	return JS_InitStandardClasses(cx, obj);
}
JSObject* JSh_InitReflect(JSContext *cx, JSObject *global)
{
	return JS_InitReflect(cx, global);
}
JSFunction* JSh_DefineFunction(JSContext *cx, JSObject *obj, const char *name, JSNative call, unsigned nargs, unsigned attrs)
{
	return JS_DefineFunction(cx, obj, name, call, nargs, attrs);
}

void JSh_ReportError(JSContext* cx, const char* sErr)
{
    JS_ReportError(cx, "%s", sErr);
}
int JSh_GetErroReportLineNo(JSErrorReport* report)
{
	return report->lineno;
}
const char* JSh_GetErroReportFileName(JSErrorReport* report)
{
	if (!report->filename)
		return "no_file_name";
	else
		return report->filename;
}
JSObject* JSh_NewArrayObject(JSContext *cx, int length, jsval *vector)
{
	return JS_NewArrayObject(cx, length, vector);
}
bool JSh_IsArrayObject(JSContext *cx, JSObject *obj)
{
	return !!JS_IsArrayObject(cx, obj);
}
// return -1: fail
int JSh_GetArrayLength(JSContext *cx, JSObject *obj)
{
	uint32_t length = 0;
	if (!JS_GetArrayLength(cx, obj, &length))
		return -1;
	return (int)length;
}
bool JSh_GetElement(JSContext *cx, JSObject *obj, uint32_t index, jsval* val)
{
	JS::RootedValue v(cx);
	JS_GetElement(cx, obj, index, &v);
	*val = v.get();
	return true;
}
bool JSh_SetElement(JSContext *cx, JSObject *obj, uint32_t index, jsval* pVal)
{
	JS::RootedValue arrElement(cx);
	arrElement = *pVal;
	return JS_SetElement(cx, obj, index, &arrElement);
}

bool JSh_GetProperty(JSContext *cx, JSObject *obj, const char* name, jsval* val)
{
    JS::RootedValue v(cx);
    JS_GetProperty(cx, obj, name, &v);
    *val = v.get();
	return true;
}
bool JSh_SetProperty(JSContext *cx, JSObject *obj, const char* name, jsval* pVal)
{
    JS::RootedValue arrElement(cx);
	arrElement = *pVal;
	return JS_SetProperty(cx, obj, name, arrElement);
}
bool JSh_GetUCProperty(JSContext *cx, JSObject *obj, jschar* name, int nameLen, jsval* val)
{
    JS::RootedValue v(cx);
    JS_GetUCProperty(cx, obj, name, nameLen, &v);
    *val = v.get();
	return true;
}
bool JSh_SetUCProperty(JSContext *cx, JSObject *obj, jschar* name, int nameLen, jsval* pVal)
{
    JS::RootedValue arrElement(cx);
	arrElement = *pVal;
	return JS_SetUCProperty(cx, obj, name, nameLen, arrElement);
}

// new a JSClass with specified flag and finalizer
JSClass* JSh_NewClass(const char* name, unsigned int flag, JSFinalizeOp finalizeOp)
{
	JSClass* cls = new JSClass();
	memcpy(cls, &global_class, sizeof(JSClass));
	int len = strlen(name);
	char* pName = (char*)malloc(len + 1);
	memcpy(pName, name, len);
	pName[len] = 0;
	cls->name = pName;
	cls->flags = flag;
	cls->finalize = finalizeOp;
	return cls;
}

// init a class with default value
JSObject* JSh_InitClass(JSContext* cx, JSObject* glob, JSClass* jsClass)
{
	JSObject* obj = JS_InitClass(cx, glob,
		NULL, /* parentProto*/
		jsClass, /* JSClass*/
		NULL, /* constructor*/
		0, /* constructor nargs */
		NULL, /* JSPropertySpec* */
		NULL, /* JSFunctionSpec* */
		NULL, /* static JSPropertySpec* */
		NULL /* static JSFunctionSpec* */
		);
	return obj;
}

void JSh_GC(JSRuntime *rt)
{
	JS_GC(rt);
}
void JSh_MaybeGC(JSContext *cx)
{
	JS_MaybeGC(cx);
}
bool JSh_EvaluateScript(JSContext *cx, JSObject *obj,
	const char *bytes, unsigned length,
	const char *filename, unsigned lineno,
	jsval *rval)
{
	return JS_EvaluateScript(cx, obj, bytes, length, filename, lineno, rval);
}

////////////////////////////////////////////////////////////////////////////////////
// new a class and assign it a class proto

JSObject* JSh_NewObjectAsClass(JSContext* cx, JSObject* glob, const char* className, JSFinalizeOp finalizeOp)
{
	JS::RootedValue nsval(cx);



	JS_GetProperty(cx, glob, className, &nsval);
	JSObject* jsObject = JSVAL_TO_OBJECT(nsval);
	if (jsObject == 0)
		return 0;

	JS_GetProperty(cx, jsObject, "prototype", &nsval);
	JSObject* proto = JSVAL_TO_OBJECT(nsval);
	if (proto == 0)
		return 0;

	JSClass* jsClass = &qiucw_class;
	jsClass->finalize = finalizeOp;
	JSObject* obj = JS_NewObject(cx, jsClass, proto, 0/* parentProto */);
	return obj;
}

JSObject* JSh_NewObject(JSContext *cx, JSClass *clasp, JSObject *proto, JSObject *parent)
{
	return JS_NewObject(cx, clasp, proto, parent);
}

JSObject* JSh_NewMyClass(JSContext *cx, JSFinalizeOp finalizeOp)
{
	JSClass* jsClass = &qiucw_class;
	jsClass->finalize = finalizeOp;
	JSObject* obj = JS_NewObject(cx, jsClass, 0/* proto */, 0/* parentProto */);
	return obj;
}

unsigned int JSh_ArgvTag(JSContext* cx, jsval* vp, int i)
{
    // JSValueTag
    jsval& val = JS_ARGV(cx, vp)[i];
    return val.data.s.tag;
}

bool JSh_ArgvBool(JSContext* cx, jsval* vp, int i) { return JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[i]); }
double JSh_ArgvDouble(JSContext* cx, jsval* vp, int i) { return JSVAL_TO_DOUBLE(JS_ARGV(cx, vp)[i]); }
int JSh_ArgvInt(JSContext* cx, jsval* vp, int i) { return JSVAL_TO_INT(JS_ARGV(cx, vp)[i]); }
const jschar* JSh_ArgvString(JSContext* cx, jsval* vp, int i)
{
	JSString* jsStr = JSVAL_TO_STRING(JS_ARGV(cx, vp)[i]);
    return getMarshalStringFromJSString(cx, jsStr);
}
const char* JSh_ArgvStringUTF8(JSContext* cx, jsval* vp, int i)
{
	JSString* jsStr = JSVAL_TO_STRING(JS_ARGV(cx, vp)[i]);
	const char* rt = JS_EncodeStringToUTF8(cx, jsStr);
	return rt;
}
JSObject* JSh_ArgvObject(JSContext* cx, jsval* vp, int i)
{
	jsval* pVal = &(JS_ARGV(cx, vp)[i]);
	if (pVal->isObject())
		return JSVAL_TO_OBJECT(*pVal);
	else
		return 0;
}
JSFunction* JSh_ArgvFunction(JSContext* cx, jsval* vp, int i) {
	jsval val = (JS_ARGV(cx, vp)[i]);
	if (!val.isObject())
		return 0;

	JSObject* obj = JSVAL_TO_OBJECT(val);
	if (!JS_ObjectIsFunction(cx, obj))
		return 0;

	return JS_ValueToFunction(cx, JS::RootedValue(cx, val));
}

bool JSh_ArgvFunctionValue(JSContext* cx, jsval* vp, int i, jsval* pval)
{
	JS::RootedValue nsval(cx);
	nsval = (JS_ARGV(cx, vp)[i]);

	JS::RootedValue ns(cx);
	if (!JS_ConvertValue(cx, nsval, JSTYPE_FUNCTION, &ns))
		return false;

	*pval = ns;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// returns

void JSh_SetRvalBool(JSContext* cx, jsval* vp, bool value) { JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(value)); }
void JSh_SetRvalDouble(JSContext* cx, jsval* vp, double value) { JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(value)); }
void JSh_SetRvalInt(JSContext* cx, jsval* vp, int value) { JS_SET_RVAL(cx, vp, INT_TO_JSVAL(value)); }
void JSh_SetRvalUInt(JSContext* cx, jsval* vp, unsigned int value) { JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(value)); }
void JSh_SetRvalString(JSContext* cx, jsval* vp, const jschar* value) {
	JSString* jsString = JS_NewUCStringCopyZ(cx, value);
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsString));
}
void JSh_SetRvalObject(JSContext* cx, jsval* vp, JSObject* value) { JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(value)); }
void JSh_SetRvalUndefined(JSContext* cx, jsval* vp) { jsval v; v.setUndefined(); JS_SET_RVAL(cx, vp, v); }
void JSh_SetRvalJSVAL(JSContext* cx, jsval* vp, jsval* value) { JS_SET_RVAL(cx, vp, *value); }

////////////////////////////////////////////////////////////////////////////////////
// generate jsval

void JSh_SetJsvalBool(jsval* vp, bool value)  { *vp = BOOLEAN_TO_JSVAL(value); }
void JSh_SetJsvalDouble(jsval* vp, double value) { *vp = DOUBLE_TO_JSVAL(value); }
void JSh_SetJsvalInt(jsval* vp, int value) { *vp = INT_TO_JSVAL(value); }
void JSh_SetJsvalUInt(jsval* vp, unsigned int value) { *vp = UINT_TO_JSVAL(value); }
void JSh_SetJsvalString(JSContext* cx, jsval* vp, const jschar* value) {
	JSString* jsString = JS_NewUCStringCopyZ(cx, value);
	*vp = STRING_TO_JSVAL(jsString);
}
void JSh_SetJsvalObject(jsval* vp, JSObject* value) { *vp = OBJECT_TO_JSVAL(value); }
void JSh_SetJsvalUndefined(jsval* vp) { vp->setUndefined(); }

bool JSh_GetJsvalBool(jsval* vp)  { return vp->toBoolean(); }
double JSh_GetJsvalDouble(jsval* vp) { return vp->toDouble(); }
int JSh_GetJsvalInt(jsval* vp) { return vp->toInt32(); }
unsigned int JSh_GetJsvalUInt(jsval* vp) { return vp->toInt32(); }
const jschar* JSh_GetJsvalString(JSContext* cx, jsval* vp) {
	JSString* jsStr = vp->toString();
    return getMarshalStringFromJSString(cx, jsStr);
}
JSObject* JSh_GetJsvalObject(jsval* vp) { return vp->toObjectOrNull(); }


JSCompartment* JSh_EnterCompartment(JSContext *cx, JSObject *target)
{
	return JS_EnterCompartment(cx, target);
}
void JSh_LeaveCompartment(JSContext *cx, JSCompartment *oldCompartment)
{
	JS_LeaveCompartment(cx, oldCompartment);
}

////////////////////////////////////////////////////////////////////////////////////

void  JSh_SetTrustedPrincipals(JSRuntime *rt, const JSPrincipals *prin)
{
	JS_SetTrustedPrincipals(rt, prin);
}
JSScript* JSh_CompileScript(JSContext *cx, JSObject* global, const char *ascii, size_t length, const char* filename, size_t lineno)
{
	JS::CompileOptions options(cx);
	options.setUTF8(true).setFileAndLine(filename, lineno);
	return JS_CompileScript(cx, JS::RootedObject(cx, global), ascii, length, options);
}
bool JSh_ExecuteScript(JSContext *cx, JSObject *obj, JSScript *script, jsval *rval)
{
	return JS_ExecuteScript(cx, obj, script, rval);
}
// this function is useless
JSFunction* JSh_GetFunction(JSContext* cx, JSObject* obj, const char* name)
{
	JS::RootedValue nsval(cx);
	JS::RootedObject ns(cx);
	if (!JS_GetProperty(cx, obj, name, &nsval))
		return 0;

	JSFunction* fun = JS_ValueToFunction(cx, nsval);
	return fun;
}
bool JSh_GetFunctionValue(JSContext* cx, JSObject* obj, const char* name, jsval* val)
{
	JS::RootedValue nsval(cx);
	if (!JS_GetProperty(cx, obj, name, &nsval))
		return false;

	if (JSVAL_IS_VOID(nsval))
		return false;

	JS::RootedValue ns(cx);
	if (!JS_ConvertValue(cx, nsval, JSTYPE_FUNCTION, &ns))
		return false;

	*val = ns;
	return true;
}
bool JSh_ObjectIsFunction(JSContext *cx, JSObject *obj)
{
	return JS_ObjectIsFunction(cx, obj);
}

bool JSh_CallFunction(JSContext *cx, JSObject *obj, JSFunction *fun, unsigned argc, jsval *argv, jsval *rval)
{
	return JS_CallFunction(cx, obj, fun, argc, argv, rval);
}
bool JSh_CallFunctionName(JSContext *cx, JSObject *obj, const char *name, unsigned argc, jsval *argv, jsval *rval)
{
	return JS_CallFunctionName(cx, obj, name, argc, argv, rval);
}
bool JSh_CallFunctionValue(JSContext *cx, JSObject *obj, jsval* fval, unsigned argc, jsval *argv, jsval *rval)
{
	return JS_CallFunctionValue(cx, obj, *fval, argc, argv, rval);
}

bool JSh_IsNative(JSObject *obj)
{
	return JS_IsNative(obj);
}
JSRuntime* JSh_GetObjectRuntime(JSObject *obj)
{
	return JS_GetObjectRuntime(obj);
}
JSObject* JSh_NewObjectWithGivenProto(JSContext *cx, const JSClass *clasp, JSObject *proto, JSObject *parent)
{
	return JS_NewObjectWithGivenProto(cx, clasp, proto, parent);
}
bool JSh_DeepFreezeObject(JSContext *cx, JSObject *obj)
{
	return JS_DeepFreezeObject(cx, obj);
}
bool JSh_FreezeObject(JSContext *cx, JSObject *obj)
{
	return JS_FreezeObject(cx, obj);
}


bool JSh_StringHasBeenInterned(JSContext *cx, JSString *str)
{
	return JS_StringHasBeenInterned(cx, str);
}

//      jsid INTERNED_STRING_TO_JSID(JSContext *cx, JSString *str)
//     {
//         return INTERNED_STRING_TO_JSID(cx, str);
//     }

// this function compile fail on mac, don't know why
//      bool JSh_CallOnce(JSCallOnceType *once, JSInitCallback func)
//     {
//         return JS_CallOnce(once, func);
//     }
int64_t JSh_Now(void)
{
	return JS_Now();
}
jsval JSh_GetNaNValue(JSContext *cx)
{
	return JS_GetNaNValue(cx);
}

jsval JSh_GetNegativeInfinityValue(JSContext *cx)
{
	return JS_GetNegativeInfinityValue(cx);
}

jsval JSh_GetPositiveInfinityValue(JSContext *cx)
{
	return JS_GetPositiveInfinityValue(cx);
}

jsval JSh_GetEmptyStringValue(JSContext *cx)
{
	return JS_GetEmptyStringValue(cx);
}

JSString* JSh_GetEmptyString(JSRuntime *rt)
{
	return JS_GetEmptyString(rt);
}

//      bool JS_ConvertValue(JSContext *cx, JS::HandleValue v, JSType type, JS::MutableHandleValue vp);
//      bool JS_ValueToObject(JSContext *cx, JS::HandleValue v, JS::MutableHandleObject objp);
JSFunction* JSh_ValueToFunction(JSContext *cx, jsval* v)
{
	JS::RootedValue rval(cx);
	rval = *v;
	JS::HandleValue hval(&rval);
	return JS_ValueToFunction(cx, hval);
}
JSFunction* JSh_ValueToConstructor(JSContext *cx, jsval* v)
{
	JS::RootedValue rval(cx);
	rval = *v;
	JS::HandleValue hval(&rval);
	return JS_ValueToConstructor(cx, hval);
}

JSString* JSh_ValueToSource(JSContext *cx, jsval v)
{
	return JS_ValueToSource(cx, v);
}
bool JSh_DoubleIsInt32(double d, int32_t *ip)
{
	return JS_DoubleIsInt32(d, ip);
}
int32_t JSh_DoubleToInt32(double d)
{
	return JS_DoubleToInt32(d);
}

uint32_t JSh_DoubleToUint32(double d)
{
	return JS_DoubleToUint32(d);
}

JSType JSh_TypeOfValue(JSContext *cx, jsval v)
{
	return JS_TypeOfValue(cx, v);
}
const char * JSh_GetTypeName(JSContext *cx, JSType type)
{
	return JS_GetTypeName(cx, type);
}

bool JSh_StrictlyEqual(JSContext *cx, jsval v1, jsval v2, bool *equal)
{
	return JS_StrictlyEqual(cx, v1, v2, equal);
}
bool JSh_LooselyEqual(JSContext *cx, jsval v1, jsval v2, bool *equal)
{
	return JS_LooselyEqual(cx, v1, v2, equal);
}
bool JSh_SameValue(JSContext *cx, jsval v1, jsval v2, bool *same)
{
	return JS_SameValue(cx, v1, v2, same);
}

bool JSh_IsBuiltinEvalFunction(JSFunction *fun)
{
	return JS_IsBuiltinEvalFunction(fun);
}
bool JSh_IsBuiltinFunctionConstructor(JSFunction *fun)
{
	return JS_IsBuiltinFunctionConstructor(fun);
}
void* JSh_GetRuntimePrivate(JSRuntime *rt)
{
	return JS_GetRuntimePrivate(rt);
}
void JSh_SetRuntimePrivate(JSRuntime *rt, void *data)
{
	return JS_SetRuntimePrivate(rt, data);
}
void JSh_BeginRequest(JSContext *cx)
{
	return JS_BeginRequest(cx);
}
void JSh_EndRequest(JSContext *cx)
{
	return JS_EndRequest(cx);
}
bool JSh_IsInRequest(JSRuntime *rt)
{
	return JS_IsInRequest(rt);
}
void JSh_SetJitHardening(JSRuntime *rt, bool enabled)
{
	return JS_SetJitHardening(rt, enabled);
}
const char * JSh_GetImplementationVersion(void)
{
	return JS_GetImplementationVersion();
}
void JSh_SetDestroyCompartmentCallback(JSRuntime *rt, JSDestroyCompartmentCallback callback)
{
	return JS_SetDestroyCompartmentCallback(rt, callback);
}
void JSh_SetDestroyZoneCallback(JSRuntime *rt, JSZoneCallback callback)
{
	return JS_SetDestroyZoneCallback(rt, callback);
}
void JSh_SetSweepZoneCallback(JSRuntime *rt, JSZoneCallback callback)
{
	return JS_SetSweepZoneCallback(rt, callback);
}
void JSh_SetCompartmentNameCallback(JSRuntime *rt, JSCompartmentNameCallback callback)
{
	return JS_SetCompartmentNameCallback(rt, callback);
}
void JSh_SetWrapObjectCallbacks(JSRuntime *rt, const JSWrapObjectCallbacks *callbacks)
{
	return JS_SetWrapObjectCallbacks(rt, callbacks);
}
void JSh_SetCompartmentPrivate(JSCompartment *compartment, void *data)
{
	return JS_SetCompartmentPrivate(compartment, data);
}
void* JSh_GetCompartmentPrivate(JSCompartment *compartment)
{
	return JS_GetCompartmentPrivate(compartment);
}
void JSh_SetZoneUserData(JS::Zone *zone, void *data)
{
	return JS_SetZoneUserData(zone, data);
}
void* JSh_GetZoneUserData(JS::Zone *zone)
{
	return JS_GetZoneUserData(zone);
}

//      bool JS_WrapObject(JSContext *cx, JS::MutableHandleObject objp);
//      bool JS_WrapValue(JSContext *cx, JS::MutableHandleValue vp);
//      bool JS_WrapId(JSContext *cx, jsid *idp);
//      JSObject* JS_TransplantObject(JSContext *cx, JS::Handle<JSObject*> origobj, JS::Handle<JSObject*> target);
//      bool JS_RefreshCrossCompartmentWrappers(JSContext *cx, JSObject *ob);
//      void JS_IterateCompartments(JSRuntime *rt, void *data, JSIterateCompartmentCallback compartmentCallback);
// 
// 
//      bool JS_ResolveStandardClass(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> id, bool *resolved);
//      bool JS_EnumerateStandardClasses(JSContext *cx, JS::Handle<JSObject*> obj);
//      bool JS_GetClassObject(JSContext *cx, JSObject *obj, JSProtoKey key, JSObject **objp);
//      bool JS_GetClassPrototype(JSContext *cx, JSProtoKey key, JSObject **objp);

JSProtoKey JSh_IdentifyClassPrototype(JSContext *cx, JSObject *obj)
{
	return JS_IdentifyClassPrototype(cx, obj);
}

JSObject* JSh_ThisObject(JSContext* cx, jsval* vp)
{
	return JS_THIS_OBJECT(cx, vp);
}

bool JSh_AddValueRoot(JSContext *cx, jsval *vp)
{
	return JS_AddValueRoot(cx, vp);
}
bool JSh_AddStringRoot(JSContext *cx, JSString **rp)
{
	return JS_AddStringRoot(cx, rp);
}
bool JSh_AddObjectRoot(JSContext *cx, JSObject **rp)
{
	return JS_AddObjectRoot(cx, rp);
}
bool JSh_AddNamedValueRoot(JSContext *cx, jsval *vp, const char *name)
{
	return JS_AddNamedValueRoot(cx, vp, name);
}
bool JSh_AddNamedValueRootRT(JSRuntime *rt, jsval *vp, const char *name)
{
	return JS_AddNamedValueRootRT(rt, vp, name);
}
bool JSh_AddNamedStringRoot(JSContext *cx, JSString **rp, const char *name)
{
	return JS_AddNamedStringRoot(cx, rp, name);
}
bool JSh_AddNamedObjectRoot(JSContext *cx, JSObject **rp, const char *name)
{
	return JS_AddNamedObjectRoot(cx, rp, name);
}
bool JSh_AddNamedScriptRoot(JSContext *cx, JSScript **rp, const char *name)
{
	return JS_AddNamedScriptRoot(cx, rp, name);
}
void JSh_RemoveValueRoot(JSContext *cx, jsval *vp)
{
	return JS_RemoveValueRoot(cx, vp);
}
void JSh_RemoveStringRoot(JSContext *cx, JSString **rp)
{
	return JS_RemoveStringRoot(cx, rp);
}
void JSh_RemoveObjectRoot(JSContext *cx, JSObject **rp)
{
	return JS_RemoveObjectRoot(cx, rp);
}
void JSh_RemoveScriptRoot(JSContext *cx, JSScript **rp)
{
	return JS_RemoveScriptRoot(cx, rp);
}
void JSh_RemoveValueRootRT(JSRuntime *rt, jsval *vp)
{
	return JS_RemoveValueRootRT(rt, vp);
}
void JSh_RemoveStringRootRT(JSRuntime *rt, JSString **rp)
{
	return JS_RemoveStringRootRT(rt, rp);
}
void JSh_RemoveObjectRootRT(JSRuntime *rt, JSObject **rp)
{
	return JS_RemoveObjectRootRT(rt, rp);
}
void JSh_RemoveScriptRootRT(JSRuntime *rt, JSScript **rp)
{
	return JS_RemoveScriptRootRT(rt, rp);
}
void JSh_SetNativeStackQuota(JSRuntime *cx, size_t systemCodeStackSize, size_t trustedScriptStackSize, size_t untrustedScriptStackSize)
{
	JS_SetNativeStackQuota(cx, systemCodeStackSize, trustedScriptStackSize, untrustedScriptStackSize);
}


#ifdef ENABLE_JS_DEBUG
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void JSh_EnableDebugger(JSContext* cx, JSObject* global, const char** src_searchpath, int nums, int port){

	jsdebugger::getInstance()->Start(cx, global, &global_class, src_searchpath, nums, port);

}

void JSh_UpdateDebugger(){
	jsdebugger::getInstance()->update(1.0f);
}

void JSh_CleanupDebugger(){
	jsdebugger::Clean();
}

bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file)
{
	return jsdebugger::getInstance()->runScript(script_file, global, cx);
}

void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file)
{
	return jsdebugger::getInstance()->compileScript(script_file, global, cx);
}
MOZ_API void JSh_DumpBacktrace( JSContext* cx )
{
    js_DumpBacktrace(cx);
}
#else
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void JSh_EnableDebugger(JSContext* cx, JSObject* global, const char** src_searchpath, int nums, int port) {}
void JSh_UpdateDebugger() {}
void JSh_CleanupDebugger() {}
bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file) { return false;}
void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file) {}
MOZ_API void JSh_DumpBacktrace( JSContext* cx ) {}
#endif


//
// some useful function
//
MOZ_API PPV* JSh_NewPPointer(PPV pValue) 
{
    PPV* p = new PPV;
    *p = pValue;
    return p;
}
MOZ_API void JSh_DelPPointer(PPV* p)
{
    if (p)
        delete p;
}
MOZ_API void JSh_SetPPointer(PPV* p, PPV pValue)
{
    if (p) {
        *p = pValue;
    }
}

MOZ_API PPV JSh_GetPPointer(PPV* p)
{
    if (p) {
        return *p;
    }
    return 0;
}

MOZ_API JS::Heap<JSObject*>* JSh_NewHeapObject(JSObject* obj)
{
    return new JS::Heap<JSObject*>(obj);
}

MOZ_API void JSh_DelHeapObject(JS::Heap<JSObject*>* heapObj)
{
    delete heapObj;
}

MOZ_API void JSh_SetGCCallback(JSRuntime *rt, JSGCCallback cb, void *data)
{
    JS_SetGCCallback(rt, cb, data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct SplitUtil 
{
    SplitUtil(char* s, const char* sp) { str = s; splitStr = sp;}
    char* str;
    const char* splitStr;
    const char* next() {
        char* buf = strstr(str, splitStr);
        if (buf) {
            buf[0] = 0;
            const char* ret = str;
            str = buf + strlen(splitStr);
            return ret;
        }
        if (str[0])
        {
            const char* ret = str;
            str += strlen(str);
            return ret;
        }
        return 0;
    }
};
MOZ_API JSObject* GetJSTableByName(char* name)
{
    SplitUtil split(name, ".");
    const char* n = split.next();
    if (!n) return 0;

    JS::RootedObject obj(g_cx, g_global);
    JS::RootedValue val(g_cx);
    while (n)
    {
        JS_GetProperty(g_cx, obj, n, &val);
        if (val.isObject()) 
        {
            obj = &val.toObject();
            n = split.next();
        }
        else
            return 0;
    }
    return obj;
}
MOZ_API void InitPersistentObject(JSRuntime* rt, JSContext* cx, JSObject* global, JSFinalizeOp finalizer)
{
    g_rt = rt;
    g_cx = cx;
    g_global = global;
    g_finalizer = finalizer;
}

//
// 创建一个JS类对象
// 返回jsObj, nativeObj
//
MOZ_API bool NewJSClassObject(char* name, JSObject** retJSObj, JSObject** retNativeObj, JSObject* objRef)
{
    JSObject* _t;
    
    if (_t = GetJSTableByName(name))
    {
        JS::RootedObject tableObj(g_cx, _t);
        if (_t = JSh_NewObjectAsClass(g_cx, tableObj, "ctor", 0))
        {
            JS::RootedObject jsObj(g_cx, _t);
            JS::RootedObject nativeObj(g_cx, JSh_NewMyClass(g_cx, g_finalizer));
            JS::RootedValue val(g_cx);
            val.setObject(*nativeObj);
            JS_SetProperty(g_cx, jsObj, "__nativeObj", val);

            *retJSObj = jsObj;
            *retNativeObj = nativeObj;

            if (objRef)
            {
                val.setObject(*jsObj);
                JS_SetProperty(g_cx, objRef, "Value", val);
            }
            return true;
        }
    }
    return false;
}
MOZ_API void SetVector2(JSObject* jsObj, float x, float y, JSObject* objRef)
{
    JS::RootedValue val(g_cx);
    JS::RootedObject obj(g_cx, jsObj);

    val.setDouble((double)x);
    JS_SetProperty(g_cx, obj, "x", val);

    val.setDouble((double)y);
    JS_SetProperty(g_cx, obj, "y", val);

    if (objRef)
    {
        val.setObject(*jsObj);
        JS_SetProperty(g_cx, objRef, "Value", val);
    }
}

MOZ_API void SetVector3(JSObject* jsObj, float x, float y, float z, JSObject* objRef)
{
    JS::RootedValue val(g_cx);
    JS::RootedObject obj(g_cx, jsObj);

    val.setDouble((double)x);
    JS_SetProperty(g_cx, obj, "x", val);

    val.setDouble((double)y);
    JS_SetProperty(g_cx, obj, "y", val);

    val.setDouble((double)z);
    JS_SetProperty(g_cx, obj, "z", val);

    if (objRef)
    {
        val.setObject(*jsObj);
        JS_SetProperty(g_cx, objRef, "Value", val);
    }
}

// void jsGCCallback (JSRuntime *rt, JSGCStatus status, void *data)
// {
// }

// 
// 返回0表示没错
//
MOZ_API int InitJSEngine(JSErrorReporter er)
{
    JSRuntime* rt;
    JSContext* cx;
    JSObject* global;

    if (!JS_Init())
    {
        return 1;
    }

    rt = JS_NewRuntime(8 * 1024 * 1024, JS_NO_HELPER_THREADS);
    JS_SetNativeStackQuota(rt, 500000, 0, 0);

    cx = JS_NewContext(rt, 8192);

    // Set error reporter
    JS_SetErrorReporter(cx, er);

    JS::CompartmentOptions options;
	options.setVersion(JSVERSION_LATEST);
	global = JS_NewGlobalObject(cx, &global_class, 0/*principals*/, JS::DontFireOnNewGlobalHook, options);
        
    JSCompartment* oldCompartment = JS_EnterCompartment(cx, global);
    if (!JS_InitStandardClasses(cx, global))
    {
        return 2;
    }
    JS_InitReflect(cx, global);

    //JS_SetGCCallback(rt, jsGCCallback, 0/* user data */);

    // 赋值全局变量
    g_rt = rt;
    g_cx = cx;
    g_global = global;
    return 0;
}

