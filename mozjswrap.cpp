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

////////////////////////////////////////////////////////////////////////////////////
// new a class and assign it a class proto

JSObject* JSh_NewObjectAsClass(JSContext* cx, JSObject* glob, const char* className, JSFinalizeOp finalizeOp)
{
	JS::RootedValue nsval(cx);
	JS_GetProperty(cx, glob, className, &nsval);
	if (nsval.isObject())
	{
		JS::RootedObject jsObject(cx, &nsval.toObject());
		JS_GetProperty(cx, jsObject, "prototype", &nsval);
		if (nsval.isObject())
		{
			jsObject = &nsval.toObject();
			JSClass* jsClass = &qiucw_class;
			jsClass->finalize = finalizeOp;
			JSObject* obj = JS_NewObject(cx, jsClass, jsObject/* proto */, 0/* parentProto */);
			return obj;
		}
	}
	return 0;
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
void JSh_EnableDebugger(/*JSContext* cx, JSObject* global, */const char** src_searchpath, int nums, int port){

	jsdebugger::getInstance()->Start(g_cx, g_global, &global_class, src_searchpath, nums, port);

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
#else
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void JSh_EnableDebugger(/*JSContext* cx, JSObject* global, */const char** src_searchpath, int nums, int port) {}
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

//
// 创建一个JS类对象
// 返回jsObj, nativeObj
//
// TODO finalizer ?
MOZ_API OBJID NewJSClassObject(char* name)
{
	JS::RootedObject _t(g_cx);
    
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

            OBJID id = storeJSObject(jsObj, nativeObj);
            return id;
// 
//             *retJSObj = jsObj;
//             *retNativeObj = nativeObj;
// 
//             if (objRef)
//             {
//                 val.setObject(*jsObj);
//                 JS_SetProperty(g_cx, objRef, "Value", val);
//             }
//             return true;
        }
    }
    return 0;
}

MOZ_API bool RemoveJSClassObject(OBJID odjID)
{
    return deleteJSObject(odjID);
}
MOZ_API bool IsJSClassObjectFunctionExist(OBJID objID, const char* functionName)
{
    JS::RootedObject obj = ID2JSObj(objID);
    if (obj == 0)
        return false;

    JS::RootedValue val(g_cx);
    JS_GetProperty(g_cx, obj, functionName, &val);

    if (val.isNullOrUndefined())
        return false;

    if (!JS_ConvertValue(cx, val, JSTYPE_FUNCTION, &val))
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
    JS::RootedObject CS_obj = JSh_InitClass(cx, global, jsClass);
    JS_DefineFunction(cx, CS_obj, "Call", JSCall, 0/* narg */, 0);
    JS_DefineFunction(cx, CS_obj, "require", csEntry, 0/* narg */, 0);

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

MOZ_API JSContext* GetContext()
{
	return g_cx;
}
MOZ_API JSObject* GetGlobal()
{
	return g_global;
}
MOZ_API JSRuntime* GetRuntime()
{
	return g_rt;
}


#include "heapObjStorage.h"

#include <map>
using namespace std;

map<OBJID, MyHeapObj> mapObjs;
OBJID objID = 1; // starts from 1, 0 means nothing

OBJID storeJSObject(JS::HandleObject jsObj, JS::HandleObject nativeObj)
{
    MyHeapObj st;
    {
        st.heapJSObj = new JS::Heap<JSObject*>(jsObj);
        st.jsObj = jsObj;
        st.heapNativeObj = new JS::Heap<JSObject*>(nativeObj);
        st.nativeObj = nativeObj;
    }
    OBJID id = objID++;
    mapObjs[id] = st;
    return id;
}

OBJID jsObj2ID(JS::HandleObject nativeObj)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.begin();
    for (; it != mapObjs.end(); it++)
    {
        if (*(it->second.heapNativeObj) == nativeObj)
        {
            return it->first;
        }
    }
    return 0;
}

JSObject* ID2JSObj(OBJID id)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.find(id);
    if (it != mapObjs.end())
        return *(it->second.heapJSObj);
    return 0;
}

bool deleteJSObject(OBJID id)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.find(id);
    if (it != mapObjs.end())
        mapObjs.erase(it);
    //     map<OBJID, MyHeapObj>::iterator it = mapObjs.begin();
    //     for (; it != mapObjs.end(); it++)
    //     {
    //         if (*(it->second.heapJSObj) == jsObj)
    //         {
    // 			delete it->second.heapJSObj;
    // 			delete it->second.heapNativeObj;
    //             mapObjs.erase(it);
    //             return;
    //         }
    //     }
}

//
// for
// 1) Array
// 2) Function Arguments
// 
JS::Heap<JS::Value>* arrHeapObj = 0;
int arrHeapObjSize = 0;
int arrHeapObjLastIndex = -1;
JS::Heap<JS::Value>* makeSureArrHeapObj(int index)
{
    int size = index + 1;
    int& S = arrHeapObjSize;
    if (arrHeapObj == 0 || S < size) 
    {
        int oldS = S;
        if (S == 0)
            S = 8;
        while (S < size)
            S *= 2;

        JS::Heap<JS::Value>* arr = new JS::Heap<JS::Value>[arrHeapObjSize];
        if (index > 0)
        {
            int N = min(oldS - 1, index);
            for (int i = 0; i < N; i++)
                arr[i] = oldS[i];
        }
        arrHeapObj = arr;
    }
}

MOZ_API void moveVal2Arr(int i, JS::HandleValue val)
{
    JS::Heap<JS::Value>* arr = makeSureArrHeapObj(i);
    arr[i] = val;
    arrHeapObjLastIndex = i;
}

void clearArrObjectVal()
{
    int& index = arrHeapObjLastIndex;
    if (index >= 0)
    {
        for (int i = 0; i <= index; i++)
        {
            arrHeapObj[i] = 0;
        }
        index = -1;
    }
}

std::map<OBJID, JSObject**> mapObjectRoot;
MOZ_API bool addObjectRoot( OBJID id )
{
    if (mapObjectRoot.find(id) != mapObjectRoot.end())
    {
        return true;
    }
    JSObject* jsObj = ID2JSObj(id);
    if (jsObj == 0)
        return false;
    JSObject** pjsObj = new JSObject*;
    *pjsObj = jsObj;
    bool ret = JS_AddObjectRoot(g_cx, pjsObj);
    mapObjectRoot[id] = pjsObj;
    return ret;
}

MOZ_API bool removeObjectRoot( OBJID id )
{
    std::map<OBJID, JSObject**>::iterator it = mapObjectRoot.find(id);
    if (it != mapObjectRoot.end())
    {
        JSObject** pjsObj = it->second;
        JS_RemoveObjectRoot(g_cx, pjsObj);
        delete pjsObj;
        mapObjectRoot.erase(it);
        return true;
    }
    return false;
}

// TODO 所有存在这里的都要立即删除
std::map<int, JS::Heap<JS::Value> > mapHeapVal;
int mapHeapVal_i = 1;

MOZ_API void removeHeapMapVal( int index )
{
    std::map<int, JS::Heap<JS::Value> >::iterator it = mapHeapVal.find(index);
    if (it != mapHeapVal.end())
        mapHeapVal.erase(it);
}

MOZ_API int moveVal2HeapMap()
{
    mapHeapVal[mapHeapVal_i] = valTemp;
    return mapHeapVal_i++;
}

bool moveValFromMap2Arr(int iMap, int iArr)
{
    std::map<int, JS::Heap<JS::Value> >::iterator it = mapHeapVal.find(iMap);
    if (it != mapHeapVal.end())
    {
        JS::RootedValue val(g_cx, it->second.get());
        moveVal2Arr(iArr, val);
        return true;
    }
    return false;
}

MOZ_API bool setProperty( OBJID id, const char* name, int iMap )
{
    JS::RootedObject jsObj(g_cx, ID2JSObj(id));
    if (jsObj == 0)
        return false;

    std::map<int, JS::Heap<JS::Value> >::iterator it = mapHeapVal.find(iMap);
    if (it == mapHeapVal.end())
        return false;

    JS::RootedValue val(g_cx, it->second);
    bool ret = JS_SetProperty(g_cx, jsObj, name, val);
    return ret;
}

MOZ_API void gc()
{
    JS_GC(g_rt);
}



//
// global variables
//
//
JS::Value* g_vp = 0;
int g_argc = 0;
// 当前取到第几个参数了
int currIndex = 0;
// 实际几个参数
int actualArgc = 0;

JS::Heap<JS::Value> valFunRet;
JS::Heap<JS::Value> valTemp;

CSEntry csEntry = 0; 
bool JSCall(JSContext *cx, int argc, JS::Value *vp)
{
    g_vp = vp;
    g_argc = argc;

    int op = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
    int slot = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
    int index = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
    bool isStatic = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[3]);

    //
    // 计算参数个数，不算末尾的 undefined
    //
    // TODO check
    actualArgc = argc;
    while (actualArgc > 0 && (JS_ARGV(cx, vp))[actualArgc - 1].isUndefined())
        actualArgc--;

    currIndex = 4;
    bool ret = csEntry(op, slot, index, isStatic, actualArgc - currIndex);
    return ret;
}

int getCurrIndex()
{
    return currIndex;
}

void setCurIndex(int i)
{
    if (i >= 0 && i < actualArgc)
    {
        currIndex = i;
    }
}

unsigned int argTag( int i )
{
    if (i >= 0 && i < actualArgc)
    {
        // JSValueTag
        jsval& val = JS_ARGV(g_cx, g_vp)[i];
        return val.data.s.tag;
    }
    else
        return 0;
}

template<class T>
T val2Number(JS::RootedValue* pval)
{
    if (pval->isDouble())
        return (T)pval->toDouble();
    else
        return (T)pval->toInt32();
}

extern jschar* getMarshalStringFromJSString(JSContext* cx, JSString* jsStr);
const jschar* val2String(JS::RootedValue* pval)
{
    JSString* jsStr = pval->toString();
    return getMarshalStringFromJSString(g_cx, jsStr);
}

JS::Value& getVpVal()
{
    int i = currIndex++;
    return g_vp[i];
}

template<class T>
T getNumber(eGetType e)
{
    T ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            ret = val2Number<T>(&val);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = val2Number<T>(&v);
        }
        break;
    case GetJSFunRet:
        {
            ret = val2Number<T>(&valFunRet);
        }
        break;
    }
    return ret;
}

short           getChar    (eGetType e) { return getNumber<short>(e); }
char            getSByte   (eGetType e) { return getNumber<char>(e); }
unsigned char   getByte    (eGetType e) { return getNumber<unsigned char>(e); }
short           getInt16   (eGetType e) { return getNumber<short>(e); }
unsigned short  getUInt16  (eGetType e) { return getNumber<unsigned short>(e); }
int             getInt32   (eGetType e) { return getNumber<int>(e); }
unsigned int    getUInt32  (eGetType e) { return getNumber<unsigned int>(e); }
long long       getInt64   (eGetType e) { return getNumber<long long>(e); }
unsigned long long getUInt64  (eGetType e) { return getNumber<unsigned long long>(e); }
int             getEnum    (eGetType e) { return getNumber<int>(e); }
float           getSingle  (eGetType e) { return getNumber<float>(e); }
double          getDouble  (eGetType e) { return getNumber<double>(e); }
long long       getIntPtr  (eGetType e) { return getNumber<long long>(e); }

bool getBoolean(eGetType e) {
    bool ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            ret = val.toBoolean();
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = v.toBoolean();
        }
        break;
    case GetJSFunRet:
        {
            ret = valFunRet.toBoolean();
        }
        break;
    }
    return ret;
}
const jschar* getString(eGetType e) {
    const jschar* ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            ret = val2String(&val);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = val2String(&v);
        }
        break;
    case GetJSFunRet:
        {
            ret = val2String(&valFunRet);
        }
        break;
    }
    return ret;
}

float* floatPtr[3];
void setFloatPtr2(float* f0, float* f1)
{
    floatPtr[0] = f0;
    floatPtr[1] = f1;
}
void setFloatPtr3(float* f0, float* f1, float* f2)
{
    floatPtr[0] = f0;
    floatPtr[1] = f1;
    floatPtr[2] = f2;
}
void val2Vector2(JS::RootedValue* pval)
{
    JS::RootedObject obj(g_cx, &pval->toObject());

    JS::RootedValue val(g_cx);

    JS_GetProperty(g_cx, obj, "x", &val);
    *(floatPtr[0]) = val2Number<float>(&val);

    JS_GetProperty(g_cx, obj, "y", &val);
    *(floatPtr[1]) = val2Number<float>(&val);
}
void val2Vector3(JS::RootedValue* pval)
{
    JS::RootedObject obj(g_cx, &pval->toObject());

    JS::RootedValue val(g_cx);

    JS_GetProperty(g_cx, obj, "x", &val);
    *(floatPtr[0]) = val2Number<float>(&val);

    JS_GetProperty(g_cx, obj, "y", &val);
    *(floatPtr[1]) = val2Number<float>(&val);

    JS_GetProperty(g_cx, obj, "z", &val);
    *(floatPtr[2]) = val2Number<float>(&val);
}
bool getVector2(eGetType e)
{
    bool ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            val2Vector2(&val);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            val2Vector2(&v);
        }
        break;
    case GetJSFunRet:
        {
            val2Vector2(&valFunRet);
        }
        break;
    }
    return ret;
}
bool getVector3(eGetType e)
{
    bool ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            val2Vector3(&val);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            val2Vector3(&v);
        }
        break;
    case GetJSFunRet:
        {
            val2Vector3(&valFunRet);
        }
        break;
    }
    return ret;

}
OBJID getObject(eGetType e)
{
    OBJID ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject obj(g_cx, &val.toObject());
            ret = jsObj2ID(obj);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            JS::RootedObject jsObj2(g_cx, &v.toObject());
            ret = jsObj2ID(jsObj2);
        }
        break;
    case GetJSFunRet:
        {
            JS::RootedObject jsObj(g_cx, &valFunRet.toObject());
            ret = jsObj2ID(jsObj);
        }
        break;
    }
    return ret;
}

template<class T>
void setNumberI(eSetType e, T value)
{
    int vSet = (int)value;
    switch (e)
    {
    case SetJsval:
        valTemp.setInt32(vSet);
        break;
    case SetRval:
        JS_SET_RVAL(g_cx, g_vp, INT_TO_JSVAL(vSet));
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            v.setInt32(vSet);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}

template<class T>
void setNumberF(eSetType e, T value)
{
    double vSet = (double)value;
    switch (e)
    {
    case SetJsval:
        valTemp.setDouble(vSet);
        break;
    case SetRval:
        JS_SET_RVAL(g_cx, g_vp, DOUBLE_TO_JSVAL(vSet));
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            v.setDouble(vSet);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}

void setChar    (eSetType e, short v)           { return setNumberI<short>(e, v); }
void setSByte   (eSetType e, char v)            { return setNumberI<char>(e, v); }
void setByte    (eSetType e, unsigned char v)   { return setNumberI<unsigned char>(e, v); }
void setInt16   (eSetType e, short v)           { return setNumberI<short>(e, v); }
void setUInt16  (eSetType e, unsigned short v)  { return setNumberI<unsigned short>(e, v); }
void setInt32   (eSetType e, int v)             { return setNumberI<int>(e, v); }
void setUInt32  (eSetType e, unsigned int v)    { return setNumberI<unsigned int>(e, v); }
void setInt64   (eSetType e, long long v)       { return setNumberF<long long>(e, v); }
void setUInt64  (eSetType e, unsigned long long v) { return setNumberF<unsigned long long>(e, v); }
void setEnum    (eSetType e, int v)             { return setNumberI<int>(e, v); }
void setSingle  (eSetType e, float v)           { return setNumberF<float>(e, v); }
void setDouble  (eSetType e, double v)          { return setNumberF<double>(e, v); }
void setIntPtr  (eSetType e, long long v)       { return setNumberF<long long>(e, v); }
void setBoolean(eSetType e, bool v)
{
    bool vSet = v;
    switch (e)
    {
    case SetJsval:
        valTemp.setBoolean(vSet);
        break;
    case SetRval:
        JS_SET_RVAL(g_cx, g_vp, BOOLEAN_TO_JSVAL(vSet));
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            v.setBoolean(vSet);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}
void setString(eSetType e, const jschar* value)
{
    switch (e)
    {
    case SetJsval:
        JS::RootedString jsString = JS_NewUCStringCopyZ(g_cx, value);
        valTemp.setString(jsString);
        break;
    case SetRval:
        JS::RootedString jsString = JS_NewUCStringCopyZ(g_cx, value);
        JS_SET_RVAL(g_cx, g_vp, STRING_TO_JSVAL(jsString));
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS::RootedString jsString = JS_NewUCStringCopyZ(g_cx, value);
            v.setString(jsString);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}
void valSetVector2(JS::RootedValue* pval, float x, float y)
{
    JS::RootedObject obj(g_cx, &pval->toObject());

    JS::RootedValue val(g_cx);

    val.setDouble(x);
    JS_SetProperty(g_cx, obj, "x", val);

    val.setDouble(y);
    JS_SetProperty(g_cx, obj, "y", val);
}
void setVector2(eSetType e, float x, float y)
{
    switch (e)
    {
    case SetJsval:
        valSetVector2(pvalTemp, x, y);
        break;
    case SetRval:
        {
            JS::RootedValue val(g_cx);
            valSetVector2(&val, x, y);
            JS_SET_RVAL(g_cx, g_vp, val);
        }
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            valSetVector2(&v, x, y);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}
void valSetVector3(JS::RootedValue* pval, float x, float y, float z)
{
    JS::RootedObject obj(g_cx, &pval->toObject());

    JS::RootedValue val(g_cx);

    val.setDouble(x);
    JS_SetProperty(g_cx, obj, "x", val);

    val.setDouble(y);
    JS_SetProperty(g_cx, obj, "y", val);

    val.setDouble(z);
    JS_SetProperty(g_cx, obj, "z", val);
}
void setVector3(eSetType e, float x, float y, float z)
{
    switch (e)
    {
    case SetJsval:
        valSetVector3(pvalTemp, x, y, z);
        break;
    case SetRval:
        {
            JS::RootedValue val(g_cx);
            valSetVector3(&val, x, y, z);
            JS_SET_RVAL(g_cx, g_vp, val);
        }
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            valSetVector3(&v, x, y, z);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}
void setObject(eSetType e, OBJID id)
{
    // TODO
    // Check: when id == 0
    JS::RootedObject jsObj(g_cx, ID2JSObj(id));
    switch (e)
    {
    case SetJsval:
        valTemp.setObjectOrNull(jsObj);
        break;
    case SetRval:
        {
            JS::RootedValue val(g_cx);
            val.setObjectOrNull(jsObj);
            JS_SET_RVAL(g_cx, g_vp, val);
        }
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            val.setObjectOrNull(jsObj);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}
void setArray(eSetType e, int count)
{
    JSObject* _t = JS_NewArrayObject(g_cx, count, 0 /* jsval* */);
    JS::RootedObject arrObj(g_cx, _t);
    for (int i = 0; i < count; i++)
    {
        JS_SetElement(g_cx, arrObj, i, &arrHeapObj[i]);
    }

    // TODO clear arrHeapObj

    switch (e)
    {
    case SetJsval:
        valTemp.setObjectOrNull(arrObj);
        break;
    case SetRval:
        {
            JS::RootedValue val(g_cx);
            val.setObjectOrNull(arrObj);
            JS_SET_RVAL(g_cx, g_vp, val);
        }
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            val.setObjectOrNull(arrObj);
            JS_SetProperty(g_cx, jsObj, "Value", v);
        }
        break;
    }
}

MOZ_API bool isVector2( int i )
{
    JS::Value& val = JS_ARGV(cx, vp)[i];
    if (val.isObject())
    {
        JS::RootedObject obj = val.toObject();
        JS::RootedValue v(g_cx);
        JS_GetProperty(g_cx, obj, "_fullname", &v);
        if (v->isString())
        {
            // TODO fix memory leak
            JS::RootedString jsStr = v->toString();
            const char* str = JS_EncodeStringToUTF8(g_cx, jsStr);
            return strcmp(str, "UnityEngine.Vector2") == 0;
        }
    }
    return false;
}

MOZ_API bool isVector3( int i )
{
    JS::Value& val = JS_ARGV(cx, vp)[i];
    if (val.isObject())
    {
        JS::RootedObject obj = val.toObject();
        JS::RootedValue v(g_cx);
        JS_GetProperty(g_cx, obj, "_fullname", &v);
        if (v->isString())
        {
            // TODO fix memory leak
            JS::RootedString jsStr = v->toString();
            const char* str = JS_EncodeStringToUTF8(g_cx, jsStr);
            return strcmp(str, "UnityEngine.Vector3") == 0;
        }
    }
    return false;
}

MOZ_API void moveTempVal2Arr( int i )
{
    moveVal2Arr(i, *valTemp);
}

extern JS::Heap<JS::Value>* arrHeapObj;
MOZ_API bool callFunctionName(OBJID jsObjID, const char* functionName, int argCount)
{
    JS::RootedObject jsObj(g_cx, ID2JSObj(jsObjID));
    if (jsObj == 0)
        return false;

    // TODO clear arrHeapObj
    if (argCount == 0)
    {
        return JS_CallFunctionName(g_cx, jsObj, functionName, 0/* argc */, 0/* argv */, &valFunRet);
    }
    else
    {
        JS::Value* array = new JS::Value[argCount];
        for (int i = 0; i < argCount; i++)
            array[i] = arrHeapObj[i];

        return JS_CallFunctionName(g_cx, jsObj, functionName, argCount, array, &valFunRet);
    }
}

MOZ_API bool evaluate( const char* ascii, size_t length, const char* filename )
{
    int lineno = 1;
    JS::CompileOptions options(cx);
    options.setUTF8(true).setFileAndLine(filename, lineno);
    JS::RootedScript jsScript = JS_CompileScript(g_cx, JS::RootedObject(g_cx, g_global), ascii, length, options);
    if (jsScript == 0)
        return false;

    JS::RootedValue val(g_cx);
    if (!JS_ExecuteScript(g_cx, g_global, jsScript, &val))
        return false;

    // val 不需要

    // TODO add script root
    // 需要吗？好像不需要吧
    JS_AddNamedScriptRoot(g_cx, &jsScript, filename);

    return true;
}

MOZ_API void setRvalBool( jsval* vp, bool v )
{
    JS_SET_RVAL(g_cx, vp, BOOLEAN_TO_JSVAL(v));
}

MOZ_API void reportError( const char* err )
{
    JS_ReportError(g_cx, "%s", err);
}
