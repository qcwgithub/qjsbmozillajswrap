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

char* getMarshalStringFromJSString(JSContext* cx, JS::HandleString jsStr)
{
    char* ori = JS_EncodeStringToUTF8(cx, jsStr);
	int length = strlen(ori);
    char* rt = (char*)::getMarshalMemory(length + 1);
    rt[length] = 0;
    memcpy(rt, ori, length);
	JS_free(cx, (void*)ori);
    return rt;
}

JSRuntime* g_rt = 0;
JSContext* g_cx = 0;
//JSObject* g_global = 0;
mozilla::Maybe<JS::PersistentRootedObject> g_global;
OnObjCollected onObjCollected = 0;
bool shutingDown = false;
void sc_finalize(JSFreeOp* freeOp, JSObject* obj)
{
    int id = (int)(long long)JS_GetPrivate(obj);

    bool bRemoved = valueMap::removeByID(id, true);
    if (!shutingDown) Assert(bRemoved, "finalize remove fail");

    // notify C#
    onObjCollected(id);
}

static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL,
	0,0,0,JS_GlobalObjectTraceHook,
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

};

static JSClass debug_global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL,
	0,0,0,JS_GlobalObjectTraceHook,
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

};

static JSClass normal_class =
{
	"qiucw_n", 0,
	JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, NULL,
	0,0,0,0,
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
static JSClass class_with_finalizer =
{
    "qiucw_f", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_DeletePropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sc_finalize,
	0,0,0,0,
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
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

#ifdef ENABLE_JS_DEBUG
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void enableDebugger(const char** src_searchpath, int nums, int port){
	jsdebugger::getInstance()->Start(g_cx, &debug_global_class, src_searchpath, nums, port);

}

void updateDebugger(){
	jsdebugger::getInstance()->update(1.0f);
}

void cleanupDebugger(){
	jsdebugger::Clean();
}

// bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file)
// {
// 	return jsdebugger::getInstance()->runScript(script_file, global, cx);
// }
// 
// void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file)
// {
// 	return jsdebugger::getInstance()->compileScript(script_file, global, cx);
// }
#else
////////////////////////////////////////////////////////////////////////////////////
// debugger api
void enableDebugger(const char** src_searchpath, int nums, int port) {}
void updateDebugger() {}
void cleanupDebugger() {}
// bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file) { return false;}
// void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file) {}
//MOZ_API void JSh_DumpBacktrace( JSContext* cx ) {}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

variableLengthArray<char> SplitUtil::sArr;

// TODO cache?
JSObject* GetJSTableByName(char* name)
{
    SplitUtil split(name, ".");
    const char* n = split.next();
    if (!n) return 0;

    JS::RootedObject obj(g_cx, g_global.ref().get());
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
        {
            Assert(false, "GetJSTableByName fail");
            return 0;
        }
    }
    return obj;
}

JSObject* getObjCtorPrototype(JS::HandleObject obj)
{
	JS::RootedValue val(g_cx);
	JS_GetProperty(g_cx, obj, "ctor", &val);
	if (val.isObject())
	{
		JS::RootedObject rO(g_cx, &val.toObject());
		JS_GetProperty(g_cx, rO, "prototype", &val);
		if (val.isObject())
		{
			return &val.toObject();
		}
	}
    Assert(false, "getObjCtorPrototype fail");
	return 0;
}

/*

why do we need to attach finalizeOp on object?
there are C# objects stored, we have to tell C# to release those objects when js object collected

there are now 2 places calling this function:
1) C# object constructor
   example: UnityEngine_GameObjectGenerated.GameObject_GameObject1
   when constructor is called, there is already a js object in hand, so, yes, add a finalizeOp to it

2) JSDataExchangeMgr.setObject 
   it will call createJSClassObject

*/
void attachFinalizerObject(MAPID id)
{
    MGETOBJ0(id, obj);

	//
	// obj.__just_for_finalize = fObj
	//
    JS::RootedObject fObj(g_cx, JS_NewObject(g_cx, &class_with_finalizer, JS::NullPtr(), JS::NullPtr()));
    JS_SetPrivate(fObj, (void*)id);
    JS::RootedValue val(g_cx);
    val.setObject(*fObj);
    JS_DefineProperty(g_cx, obj, "__just_for_finalize", val, 0/* getter */, 0/* setter */, 0/* attr */);

    valueMap::setHasFinalizeOp(id, true);
}

/*
几个操作说明，他们达到相同的目的

A. JS: new GameObject.ctor()
-------------------------------------
1) C#: GameObject_GameObject1
2)   C#: 对传进来的 _this 调用 attachFinalizerObject
3)   C: 添加 jsID -> csObj 的对应关系


B. 当C#中某个操作需要返回一个对象给JS时：createJSClassObject
-------------------------------------
1) C#: JSDataExchangeMgr.setObject
2)   C#: 调用 createJSClassObject("GameObject")
3)     C: 创建一个普通对象 jsObj，设置 proto = GameObject.ctor.prototype
4)      C: 对 jsObj 设置 attachFinalizerObject


C. 当 JSSerizlizer 需要生成一个对象时：newJSClassObject
-------------------------------------
1) 调用 newJSClassObject("GameObject")
2) 接下去 A 的步骤

*/

//
// 创建一个JS类对象
// 返回jsObj
//
//
// 假设 name = GameObject
// 操作：
// 新增一个JS对象，将其 proto 设置为 GameObject.ctor.prototype
// 并设置 finalizer
// 并把这个对象存起来  返回ID
// 
// 注意：这个操作有别于 new GameObject.ctor()，new GameObject.ctor() 是会调用到C#去创建 C#对象，我们这里只是单纯的JS对象
// 
JSObject* _createJSClassObject(char* name)
{
    JS::RootedObject _t(g_cx);

    if (_t = GetJSTableByName(name))
    {
        JS::RootedObject tableObj(g_cx, _t);
        JS::RootedObject prototypeObj(g_cx, getObjCtorPrototype(tableObj));
        
		JSObject* jsObj = JS_NewObject(g_cx, &normal_class, prototypeObj/* proto */, JS::NullPtr()/* parentProto */);
        return jsObj;
    }
    return 0;
}
// to create a C# object
MOZ_API MAPID createJSClassObject(char* name)
{
    JS::RootedObject jsObj(g_cx, _createJSClassObject(name));
    if (jsObj)
    {
		JS::Value _v; 
		_v.setObject(*jsObj);
        JS::RootedValue val(g_cx, _v);
        int id = valueMap::add(val, 1);
        attachFinalizerObject(id);
        return id;
    }
    return 0;
}
// 如果说 JS ctor不会调用到C# 那么 createXXX newXXX 结果一样
MOZ_API int newJSClassObject(const char* name)
{
    JS::RootedString jsString(g_cx, JS_NewStringCopyZ(g_cx, name));

    JS::RootedValue val(g_cx);
    val.setString(jsString);

    JS::RootedValue _rval(g_cx);
	JS::RootedObject roGlobal(g_cx, g_global.ref().get());
    if (JS_CallFunctionName(g_cx, roGlobal, "jsb_CallObjectCtor", JS::HandleValueArray::fromMarkedLocation(1, val.address()), &_rval))
    {
        //JS::RootedValue rval(g_cx, _rval);
        if (_rval.isObject())
        {
            return valueMap::add(_rval, 2);
            // not need to add finalizeOp here
            // if it's pure js object, .. OK
            // if it's C# object, attachFinalizerObject will be called from C#
            // attachFinalizerObject(id);
        }
    }
    return 0;
}

// MOZ_API bool IsJSClassObjectFunctionExist(OBJID objID, const char* functionName)
// {
//     JS::RootedObject obj(g_cx, objMap::id2JSObj(objID));
//     if (obj == 0)
//         return false;
// 
//     JS::RootedValue val(g_cx);
//     JS_GetProperty(g_cx, obj, functionName, &val);
// 
//     if (val.isNullOrUndefined())
//         return false;
// 
//     if (!JS_ConvertValue(g_cx, val, JSTYPE_FUNCTION, &val))
//         return false;
//     return true;
// }

void registerCS(JSNative req)
{
    JS::RootedObject CS_obj(g_cx, JS_NewObject(g_cx, &normal_class, JS::NullPtr(), JS::NullPtr()));
    JS::RootedValue val(g_cx);
    val.setObject(*CS_obj);

	JS::RootedObject roGlobal(g_cx, g_global.ref().get());
    JS_DefineProperty(g_cx, roGlobal, "CS", val, 0/* getter */, 0/* setter */, 0/* attr */);
    JS_DefineFunction(g_cx, CS_obj, "Call", JSCall, 0/* narg */, 0);
    JS_DefineFunction(g_cx, CS_obj, "require", req, 0/* narg */, 0);

	ppCSObj = new JS::Heap<JSObject*>();
    *(ppCSObj) = CS_obj;
    //*ppCSObj = CS_obj;
    JS::AddObjectRoot(g_cx, ppCSObj);
}

void myJSTraceDataOp(JSTracer *trc, void *data)
{
    valueMap::trace(trc);
}

// bool js_print(JSContext *cx, unsigned argc, JS::Value *vp)
// {
// 	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
//     if (args.get(0).isString())
//     {
//         JS::RootedString jsStr(cx, args.get(0).toString());
//         const char* _buffer = JS_EncodeStringToUTF8(cx, jsStr);
//         if (_buffer)
//         {
//             printf("C: %s\n", _buffer);
//             JS_free(cx, (void*)_buffer);
//         }
//     }
//     return true;
// }

// Just a wrapper around JSPrincipals that allows static construction.
class CCJSPrincipals : public JSPrincipals
{
public:
	explicit CCJSPrincipals(int rc = 0)
		: JSPrincipals()
	{
		refcount = rc;
	}
};

static CCJSPrincipals shellTrustedPrincipals(1);

static bool
	CheckObjectAccess(JSContext *cx)
{
	return true;
}

static JSSecurityCallbacks securityCallbacks = {
	CheckObjectAccess,
	NULL
};
// 
// return 0: success
//
JSCompartment* oldCompartment = 0;
JSErrorReporter oldErrorReporter = 0;
bool g_firstInit = true;
MOZ_API int InitJSEngine(JSErrorReporter er, 
                         CSEntry entry, 
                         JSNative req, 
                         OnObjCollected onObjCollected, 
                         JSNative print)
{
    JSRuntime*& rt = g_rt;
    JSContext*& cx = g_cx;

	if (g_firstInit)
	{
		g_firstInit = false;

		if (!JS_Init())
		{
			return 1;
		}

		rt = JS_NewRuntime(8 * 1024 * 1024
#ifdef SPIDERMONKEY31
			, JS_NO_HELPER_THREADS
#endif
			);
		JS_SetGCParameter(rt, JSGC_MAX_BYTES, 0xffffffff);

		JS_SetTrustedPrincipals(rt, &shellTrustedPrincipals);
		JS_SetSecurityCallbacks(rt, &securityCallbacks);
		JS_SetNativeStackQuota(rt, 500000, 0, 0);

		cx = JS_NewContext(rt, 8192);
		JS::RuntimeOptionsRef(rt).setIon(true);
		JS::RuntimeOptionsRef(rt).setBaseline(true);
	}

	// Set error reporter
	oldErrorReporter = JS_SetErrorReporter(cx, er);

    JS::CompartmentOptions options;
	//options.setVersion(JSVERSION_LATEST);
	g_global.construct(cx);
	g_global.ref() = JS_NewGlobalObject(cx, &global_class, 0/*principals*/, JS::DontFireOnNewGlobalHook, options);
	//JSObject*& global = g_global;
	JS::RootedObject roGlobal(g_cx, g_global.ref().get());

    oldCompartment = JS_EnterCompartment(cx, g_global.ref().get());
    if (!JS_InitStandardClasses(cx, roGlobal))
    {
        return 2;
    }
    JS_InitReflect(cx, roGlobal);
    csEntry = entry;
	::onObjCollected = onObjCollected;

    JS_AddExtraGCRootsTracer(rt, myJSTraceDataOp, 0/* data */);

	//ppCSObj = 0;
    registerCS(req);
    JS_DefineFunction(cx, roGlobal, "print", print, 0/* narg */, 0);

    //JS_SetGCCallback(rt, jsGCCallback, 0/* user data */);
    shutingDown = false;
    return 0;
}

MOZ_API void ShutdownJSEngine(_BOOL bCleanup)
{
	// Reset
	{
		shutingDown = true;
		if (ppCSObj)
		{
			JS::RemoveObjectRoot(g_cx, ppCSObj);
			delete ppCSObj;
			ppCSObj = 0;
		}
		idErrorEntry = 0;

		//JS_SetErrorReporter(g_cx, oldErrorReporter);

		JS_RemoveExtraGCRootsTracer(g_rt, myJSTraceDataOp, 0);
		idFunRet = 0;
		idSave = 0;
		valueMap::clear();
		valueArr::clear(false);

		JS_LeaveCompartment(g_cx, oldCompartment);
		g_global.destroy();

		JS_GC(g_rt);
	}

	if (bCleanup == _TRUE)
	{
		JS_DestroyContext(g_cx);
		JS_DestroyRuntime(g_rt);
		JS_ShutDown();

		g_cx = 0;
		g_rt = 0;
	}
}

MOZ_API void setProperty( MAPID id, const char* name, MAPID valueID )
{
    MGETOBJ0(id, jsObj);

    JS::RootedValue valValue(g_cx);
    if (!valueMap::getVal(valueID, &valValue))
    {
        return;
    }

    JS_SetProperty(g_cx, jsObj, name, valValue);
}

MOZ_API void getElement(MAPID id, int i)
{
    MGETOBJ0(id, obj);

    JS::RootedValue val(g_cx);
    if (!JS_GetElement(g_cx, obj, i, &val))
    {
        Assert(false, "JS_GetElement fail");
        return;
    }

    idSave = valueMap::add(val, 3);
    //return _TRUE;
}

MOZ_API int getArrayLength(MAPID id)
{
    JS::RootedValue valObj(g_cx);
    if (!valueMap::getVal(id, &valObj))
        return false;

    JS::RootedObject jsObj(g_cx, &valObj.toObject());

    uint32_t len = 0;
    JS_GetArrayLength(g_cx, jsObj, &len);
    return (int)len;
}

MOZ_API void gc()
{
    JS_GC(g_rt);
}


MOZ_API void moveSaveID2Arr( int arrIndex )
{
    valueArr::add(arrIndex, idSave);
}
MOZ_API MAPID getSaveID( )
{
    return idSave;
}
MOZ_API void removeByID( MAPID id )
{
    valueMap::removeByID(id, false);
}
MOZ_API void moveID2Arr(int id, int arrIndex)
{
    valueArr::add(arrIndex, id);
}

JS::Heap<JSObject*>* ppCSObj = 0;
MAPID idErrorEntry = 0;
_BOOL initErrorHandler()
{
	Assert(idErrorEntry == 0);
    JS::RootedObject CS_obj(g_cx, *ppCSObj);
    JS::RootedValue val(g_cx);
    JS_GetProperty(g_cx, CS_obj, "jsFunctionEntry", &val);

    if (val.isObject() &&
        JS_ObjectIsFunction(g_cx, &val.toObject()))
    {
        JS::RootedValue fval(g_cx);
        JS_ConvertValue(g_cx, val, JSTYPE_FUNCTION, &fval);
        idErrorEntry = valueMap::add(fval, 5);
        return _TRUE;
    }
    return _FALSE;
}

#include <set>
class funArgArrayMgr
{
	static std::set<JS::Value*> setFree;
	static std::set<JS::Value*> setUsed;
	static const int LENGTH = 16; // array length
	static bool inited;
	static int maxLength;

	static JS::Value* newArray()
	{
		return new JS::Value[LENGTH];
	}
// 	static void init()
// 	{
// 		if (inited) 
// 		{
// 			return;
// 		}
// 		for (int i = 0; i < 16; i++)
// 		{
// 			setFree.insert(newArray());
// 		}
// 		inited = true;
// 	}
	static void reset()
	{
		if (inited)
		{
			setFree.insert(setUsed.begin(), setUsed.end());
			setUsed.clear();
		}
	}

public:
	static JS::Value* get(int length)
	{
// 		if (!inited)
// 		{
// 			init();
// 		}

		if (length > maxLength)
			maxLength = length;

		if (length <= LENGTH)
		{
			std::set<JS::Value*>::iterator it = setFree.begin();
			if (it != setFree.end())
			{
				JS::Value* p = *it;
				setFree.erase(it);
				setUsed.insert(p);
				return p;
			}
			else
			{
				JS::Value* p = newArray();
				setUsed.insert(p);
				return p;
			}
		}
		else
		{
			return new JS::Value[length];
		}
	}
	static void giveBack(JS::Value* arr)
	{
		std::set<JS::Value*>::iterator it = setUsed.find(arr);
		if (it != setUsed.end())
		{
			setUsed.erase(it);
			setFree.insert(arr);
		}
		else
		{
			delete[] arr;
		}
	}
};
std::set<JS::Value*> funArgArrayMgr::setFree;
std::set<JS::Value*> funArgArrayMgr::setUsed;
bool funArgArrayMgr::inited = false;
int funArgArrayMgr::maxLength = 0;

MOZ_API void callFunctionValue(MAPID jsObjID, MAPID funID, int argCount)
{
	MGETOBJ0(jsObjID, jsObj);
    if (jsObj == 0)
    {
        // no error
    }

    JS::RootedValue fval(g_cx);
    if (!valueMap::getVal(funID, &fval))
    {
        Assert(false, "callFunctionValue: get function val fail");
        return;
    }

    JS::RootedValue retVal(g_cx);
    bool ret;

    if (idErrorEntry > 0)
    {
		JS::Value* arr = funArgArrayMgr::get(argCount + 2);
        arr[0].setObjectOrNull(jsObj);
        arr[1] = fval;
        JS::RootedValue ele(g_cx);
        for (int i = 0; i < argCount; i++)
        {
            bool b = valueMap::getVal(valueArr::arr[i], &ele);
			Assert(b);
            arr[i + 2] = ele;
        }
        JS::RootedValue entryVal(g_cx);
        valueMap::getVal(idErrorEntry, &entryVal);

		//JSAutoCompartment ac(g_cx, ppCSObj);
		JS::RootedObject roCSObj(g_cx, *ppCSObj);
        ret = JS_CallFunctionValue(g_cx, roCSObj, entryVal, JS::HandleValueArray::fromMarkedLocation(argCount + 2, arr), &retVal);
		Assert(ret);
		funArgArrayMgr::giveBack(arr);
    }
    else
    {
        if (argCount == 0)
        {
            ret = JS_CallFunctionValue(g_cx, jsObj, fval, JS::HandleValueArray::empty(), &retVal);
        }
        else
        {
			JS::Value* arr = funArgArrayMgr::get(argCount);
            JS::RootedValue ele(g_cx);
            for (int i = 0; i < argCount; i++)
            {
                valueMap::getVal(valueArr::arr[i], &ele);
                arr[i] = ele;
            }

			ret = JS_CallFunctionValue(g_cx, jsObj, fval, JS::HandleValueArray::fromMarkedLocation(argCount, arr), &retVal);
			funArgArrayMgr::giveBack(arr);
        }
    }

	// clear value array
	valueArr::clear(true);

    valueMap::removeByID(idFunRet, false);
    idFunRet = valueMap::add(retVal, 6);
    return;
}


MOZ_API int incRefCount(MAPID id)
{
	return valueMap::incRefCount(id);
}
MOZ_API int decRefCount(MAPID id)
{
	return valueMap::decRefCount(id);
}
MOZ_API void setTrace(MAPID id, _BOOL bTrace)
{
    valueMap::setTrace(id, (bTrace == _TRUE));
}

MOZ_API _BOOL isTraced(MAPID id)
{
	if (valueMap::isTraced(id))
		return _TRUE;
	else
		return _FALSE;
}

MOZ_API void setTempTrace(MAPID id, bool bTempTrace)
{
    valueMap::setTempTrace(id, bTempTrace);
}

MOZ_API _BOOL evaluate( const char* ascii, int length, const char* filename )
{
	JS::CompileOptions options(g_cx);
	options.setVersion(JSVERSION_LATEST);
	options.setUTF8(true);
	options.setFileAndLine(filename, 1);

    JS::RootedObject roGlobal(g_cx, g_global.ref().get());
#ifdef SPIDERMONKEY31
    JS::RootedScript jsScript(g_cx, JS_CompileScript(g_cx, roGlobal, ascii, length, options));
#else // V33
    JS::RootedScript jsScript(g_cx);
    JS_CompileScript(g_cx, roGlobal, ascii, length, options, &jsScript);
#endif
    if (jsScript == 0)
    {
        Assert(false, "JS_CompileScript fail");
        return _FALSE;
    }

    JS::RootedValue val(g_cx);
    if (!JS_ExecuteScript(g_cx, roGlobal, jsScript, &val))
    {
        Assert(false, "JS_ExecuteScript fail");
        return _FALSE;
    }

    // TODO add script root
    //JS_AddNamedScriptRoot(g_cx, &jsScript, filename);

    return _TRUE;
}

const char* getArgString(jsval* vp, int i)
{
    JS::RootedValue val(g_cx, JS_ARGV(g_cx, vp)[i]);
    Assert(val.isString());
    //JSString* jsStr = val.toString();
	JS::RootedString jsStr(g_cx, val.toString());
    if (jsStr)
    {
        return getMarshalStringFromJSString(g_cx, jsStr);
    }
    return 0;
}

MOZ_API MAPID getObjFunction(MAPID id, const char* fname)
{
//     JS::RootedValue objVal(g_cx);
//     if (!valueMap::getVal(id, &objVal) || !objVal.isObject())
//         return 0;
// 
//     JS::RootedObject obj(g_cx, &objVal.toObject());
//     if (obj == 0)
//     {
//         return 0;
//     }

    MGETOBJ1(id, obj, 0);

    JS::RootedValue val(g_cx);
    JS_GetProperty(g_cx, obj, fname, &val);
    if (val.isObject() && 
        JS_ObjectIsFunction(g_cx, &val.toObject()))
    {
        return valueMap::addFunction(val);
    }
    // Assert(false, "getObjFunction fail");
    // no error, function is not found
    return 0;
}

MOZ_API void setRvalBool( jsval* vp, _BOOL v )
{
    JS_SET_RVAL(g_cx, vp, BOOLEAN_TO_JSVAL((v == _TRUE)));
}

MOZ_API void reportError( const char* err )
{
    JS_ReportError(g_cx, "%s", err);
}
MOZ_API int getValueMapSize()
{
	return valueMap::getMapSize();
}
MOZ_API int getValueMapIndex()
{
	return valueMap::getIndex();
}
