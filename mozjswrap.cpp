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

int getErroReportLineNo(JSErrorReport* report)
{
	return report->lineno;
}
const char* getErroReportFileName(JSErrorReport* report)
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

////////////////////////////////////////////////////////////////////////////////////
// new a class and assign it a class proto

// JSObject* JSh_NewObjectAsClass(JSContext* cx, JSObject* glob, const char* className, JSFinalizeOp finalizeOp)
// {
// 	JS::RootedValue nsval(cx);
// 	JS_GetProperty(cx, glob, className, &nsval);
// 	if (nsval.isObject())
// 	{
// 		JS::RootedObject jsObject(cx, &nsval.toObject());
// 		JS_GetProperty(cx, jsObject, "prototype", &nsval);
// 		if (nsval.isObject())
// 		{
// 			jsObject = &nsval.toObject();
// 			JSClass* jsClass = &qiucw_class;
// 			jsClass->finalize = finalizeOp;
// 			JSObject* obj = JS_NewObject(cx, jsClass, jsObject/* proto */, 0/* parentProto */);
// 			return obj;
// 		}
// 	}
// 	return 0;
// }


// 
// JSObject* JSh_NewMyClass(JSContext *cx, JSFinalizeOp finalizeOp)
// {
// 	JSClass* jsClass = &qiucw_class;
// 	jsClass->finalize = finalizeOp;
// 	JSObject* obj = JS_NewObject(cx, jsClass, 0/* proto */, 0/* parentProto */);
// 	return obj;
// }

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

#ifdef ENABLE_JS_DEBUG
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void enableDebugger(const char** src_searchpath, int nums, int port){

	jsdebugger::getInstance()->Start(g_cx, g_global, &global_class, src_searchpath, nums, port);

}

void updateDebugger(){
	jsdebugger::getInstance()->update(1.0f);
}

void cleanupDebugger(){
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
#else
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void enableDebugger(const char** src_searchpath, int nums, int port) {}
void updateDebugger() {}
void cleanupDebugger() {}
bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file) { return false;}
void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file) {}
MOZ_API void JSh_DumpBacktrace( JSContext* cx ) {}
#endif

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
// TODO cache?
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

JSObject* getObjCtorPrototype(JS::HandleObject obj)
{
	JS::RootedValue val(g_cx);
	JS_GetProperty(g_cx, obj, "ctor", &val);
	if (val.isObject())
	{
		JS_GetProperty(g_cx, &val.toObject(), "prototype", &val);
		if (val.isObject())
		{
			return &val.toObject();
		}
	}
	return 0;
}

JSObject* newObject(JS::HandleObject prototypeObj, JSFinalizeOp finalizeOp)
{
	JSClass* jsClass = &qiucw_class;
	jsClass->finalize = finalizeOp;
	JSObject* obj = JS_NewObject(g_cx, jsClass, prototypeObj/* proto */, 0/* parentProto */);
	return obj;
}

//
// 创建一个JS类对象
// 返回jsObj
//
// TODO finalizer ?
//
// 假设 name = GameObject
// 做的事情其实就是  new GameObject.ctor()
// 并把这个对象存起来  返回ID
// 
//
MOZ_API OBJID NewJSClassObject(char* name)
{
	JS::RootedObject _t(g_cx);
    
    if (_t = GetJSTableByName(name))
    {
        JS::RootedObject tableObj(g_cx, _t);
		JS::RootedObject prototypeObj(g_cx, getObjCtorPrototype(tableObj));
		JS::RootedObject jsObj(g_cx, newObject(prototypeObj, g_finalizer));

		OBJID id = objMap::add(jsObj);
		return id;

//         if (_t = JSh_NewObjectAsClass(g_cx, tableObj, "ctor", 0))
//         {
//             JS::RootedObject jsObj(g_cx, _t);
//             JS::RootedObject nativeObj(g_cx, JSh_NewMyClass(g_cx, g_finalizer));
//             JS::RootedValue val(g_cx);
//             val.setObject(*nativeObj);
//             JS_SetProperty(g_cx, jsObj, "__nativeObj", val);
// 
//             OBJID id = objMap::add(jsObj, nativeObj);
//             return id;
//         }
    }
    return 0;
}

MOZ_API bool RemoveJSClassObject(OBJID odjID)
{
    return objMap::remove(odjID);
}
MOZ_API bool IsJSClassObjectFunctionExist(OBJID objID, const char* functionName)
{
    JS::RootedObject obj(g_cx, objMap::id2JSObj(objID));
    if (obj == 0)
        return false;

    JS::RootedValue val(g_cx);
    JS_GetProperty(g_cx, obj, functionName, &val);

    if (val.isNullOrUndefined())
        return false;

    if (!JS_ConvertValue(g_cx, val, JSTYPE_FUNCTION, &val))
        return false;
    return true;
}

// 
// 返回0表示没错
//
JSCompartment* oldCompartment = 0;
extern CSEntry csEntry;
MOZ_API int InitJSEngine(JSErrorReporter er, CSEntry entry, JSNative req)
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
        
    oldCompartment = JS_EnterCompartment(cx, global);
    if (!JS_InitStandardClasses(cx, global))
    {
        return 2;
    }
    JS_InitReflect(cx, global);
    csEntry = entry;

    //
    // register CS
    //
    JSClass* jsClass = JSh_NewClass("CS", 0/* flag */, 0/* finalizer */);
    JS::RootedObject CS_obj(g_cx, JSh_InitClass(cx, global, jsClass));
    JS_DefineFunction(cx, CS_obj, "Call", JSCall, 0/* narg */, 0);
    JS_DefineFunction(cx, CS_obj, "require", req, 0/* narg */, 0);
    ppCSObj = new JSObject*;
    *ppCSObj = CS_obj;
    JS_AddObjectRoot(g_cx, ppCSObj);

    //JS_SetGCCallback(rt, jsGCCallback, 0/* user data */);

    // 赋值全局变量
    g_rt = rt;
    g_cx = cx;
    g_global = global;
    return 0;
}


MOZ_API void ShutdownJSEngine()
{
	JS_LeaveCompartment(g_cx, oldCompartment);

	// TODO
// 	JSMgr.ClearJSCSRelation();
// 	JSMgr.ClearRootedObject();
// 	JSMgr.ClearCompiledScript();

	JS_DestroyContext(g_cx);
	JS_DestroyRuntime(g_rt);
	JS_ShutDown();
}

MOZ_API bool setProperty( OBJID id, const char* name, int iMap )
{
    JS::RootedObject jsObj(g_cx, objMap::id2JSObj(id));
    if (jsObj == 0)
        return false;

    JS::Value _v;
    if (!valueMap::get(iMap, &_v))
        return false;

    JS::RootedValue val(g_cx, _v);
    bool ret = JS_SetProperty(g_cx, jsObj, name, val);
    return ret;
}

MOZ_API bool getElement(OBJID id, int i)
{
    JS::RootedObject jsObj(g_cx, objMap::id2JSObj(id));
    if (jsObj == 0)
        return false;

    JS::RootedValue val(g_cx);
    if (!JS_GetElement(g_cx, jsObj, i, &val))
        return false;

    valTemp = val;
    return true;
}

MOZ_API int getArrayLength(OBJID id)
{
    JS::RootedObject jsObj(g_cx, objMap::id2JSObj(id));
    if (jsObj == 0)
        return false;

    uint32_t len = 0;
    JS_GetArrayLength(g_cx, jsObj, &len);
    return (int)len;
}

MOZ_API void gc()
{
    JS_GC(g_rt);
}

MOZ_API void moveTempVal2Arr( int i )
{
    JS::RootedValue val(g_cx, valTemp);
    valueArr::add(i, val);
}

JSObject** ppCSObj = 0;
FUNCTIONID jsErrorEntry = 0;
bool initErrorHandler()
{
    JS::RootedObject CS_obj(g_cx, *ppCSObj);
    JS::RootedValue val(g_cx);
    JS_GetProperty(g_cx, CS_obj, "jsFunctionEntry", &val);

    if (val.isObject() &&
        JS_ObjectIsFunction(g_cx, &val.toObject()))
    {
        JS::RootedValue fval(g_cx);
        JS_ConvertValue(g_cx, val, JSTYPE_FUNCTION, &fval);
        jsErrorEntry = valueMap::add(fval);
        return true;
    }
    return false;
}

JS::Value* arrFunArg = 0;
int arrFunArg_len = 0;
JS::Value* makeSureFunArgArr(int count)
{
    if (arrFunArg == 0 ||
        arrFunArg_len < count)
    {
        if (arrFunArg) delete arrFunArg;
        arrFunArg = new JS::Value[count];
        arrFunArg_len = count;
    }
    return arrFunArg;
}

MOZ_API bool callFunctionValue(OBJID jsObjID, int funID, int argCount)
{
    JS::RootedObject jsObj(g_cx, objMap::id2JSObj(jsObjID));
    if (jsObj == 0)
    {
        // no error
    }

    JS::Value _v;
    if (!valueMap::get(funID, &_v))
    {
        return false;
    }

    JS::RootedValue fval(g_cx, _v);
    jsval retVal;
    bool ret;

    if (jsErrorEntry > 0)
    {
        JS::Value* arr = makeSureFunArgArr(argCount + 2);
        arr[0].setObjectOrNull(jsObj);
        arr[1] = fval;
        for (int i = 0; i < argCount; i++)
        {
            arr[i + 2] = valueArr::arr[i];
        }
        JS::Value entryVal;
        valueMap::get(jsErrorEntry, &entryVal);
        ret = JS_CallFunctionValue(g_cx, *ppCSObj, entryVal, argCount + 2, arr, &retVal);
    }
    else
    {
        if (argCount == 0)
        {
            ret = JS_CallFunctionValue(g_cx, jsObj, fval, 0 /* argc */, 0 /* jsval* argv */, &retVal);
        }
        else
        {
            // TODO 
            JS::Value* arr = makeSureFunArgArr(argCount);
            for (int i = 0; i < argCount; i++)
            {
                arr[i] = valueArr::arr[i];
            }

            ret = JS_CallFunctionValue(g_cx, jsObj, fval, argCount, arr, &retVal);
        }
    }
    valFunRet = retVal;
    return ret;
}

MOZ_API bool addObjectRoot(int id)
{
    return objRoot::add(id);
}
MOZ_API bool removeObjectRoot(int id)
{
    return objRoot::remove(id);
}
MOZ_API bool addValueRoot(int id)
{
    JS::Value _v;
    if (!valueMap::get(id, &_v))
        return false;

    JS::RootedValue val(g_cx, _v);

    return valueRoot::add(val);
}
MOZ_API bool removeValueRoot(int id)
{
    return valueRoot::remove(id);
}

MOZ_API bool evaluate( const char* ascii, size_t length, const char* filename )
{
    int lineno = 1;
    JS::CompileOptions options(g_cx);
    options.setUTF8(true).setFileAndLine(filename, lineno);
    JS::RootedScript jsScript(g_cx, JS_CompileScript(g_cx, JS::RootedObject(g_cx, g_global), ascii, length, options));
    if (jsScript == 0)
        return false;

    //JS::RootedValue val(g_cx);
    jsval val;
    if (!JS_ExecuteScript(g_cx, g_global, jsScript, &val))
        return false;

    // val 不需要

    // TODO add script root
    // 需要吗？好像不需要吧
    //JS_AddNamedScriptRoot(g_cx, &jsScript, filename);

    return true;
}

const jschar* getArgString(jsval* vp, int i)
{
    JS::RootedValue val(g_cx, JS_ARGV(g_cx, vp)[i]);

    JSString* jsStr = val.toString();
    if (jsStr)
    {
        return getMarshalStringFromJSString(g_cx, jsStr);
    }
    return 0;
}

MOZ_API FUNCTIONID getObjFunction(OBJID id, const char* fname)
{
    JS::RootedObject obj(g_cx, objMap::id2JSObj(id));
    if (obj == 0)
    {
        return 0;
    }
    JS::RootedValue val(g_cx);
    JS_GetProperty(g_cx, obj, fname, &val);
    if (val.isObject() && 
        JS_ObjectIsFunction(g_cx, &val.toObject()))
    {
        FUNCTIONID id = valueMap::add(val);
        return id;
    }
    return 0;
}

MOZ_API void setRvalBool( jsval* vp, bool v )
{
    JS_SET_RVAL(g_cx, vp, BOOLEAN_TO_JSVAL(v));
}

MOZ_API void reportError( const char* err )
{
    JS_ReportError(g_cx, "%s", err);
}
