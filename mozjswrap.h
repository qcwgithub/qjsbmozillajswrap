
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

extern JSRuntime* g_rt;
extern JSContext* g_cx;
extern JSObject* g_global;

// C# 获取JS对象就是获取一个OBJID
typedef int OBJID;

struct MyHeapObj
{
    JS::Heap<JSObject*>* heapJSObj;
    JSObject* jsObj; // old obj, just remember here

    JS::Heap<JSObject*>* heapNativeObj;
    JSObject* nativeObj;
};

extern "C"{
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

	////////////////////////////////////////////////////////////////////////////////////
	// new a class and assign it a class

	MOZ_API JSObject* JSh_NewObjectAsClass(JSContext* cx, JSObject* glob, const char* className, JSFinalizeOp finalizeOp);
	MOZ_API JSObject* JSh_NewObject(JSContext *cx, JSClass *clasp, JSObject *proto, JSObject *parent);
	MOZ_API JSObject* JSh_NewMyClass(JSContext *cx, JSFinalizeOp finalizeOp);    
    MOZ_API unsigned int JSh_ArgvTag(JSContext* cx, jsval* vp, int i);
	MOZ_API bool JSh_ArgvFunctionValue(JSContext* cx, jsval* vp, int i, jsval* pval);

	MOZ_API JSScript* JSh_CompileScript(JSContext *cx, JSObject* global, const char *ascii, size_t length, const char* filename, size_t lineno);
	MOZ_API bool JSh_ExecuteScript(JSContext *cx, JSObject *obj, JSScript *script, jsval *rval);
	// this function is useless
	MOZ_API JSFunction* JSh_GetFunction(JSContext* cx, JSObject* obj, const char* name);
	MOZ_API bool JSh_GetFunctionValue(JSContext* cx, JSObject* obj, const char* name, jsval* val);
	MOZ_API JSFunction* JSh_ValueToFunction(JSContext *cx, jsval* v);
	MOZ_API JSFunction* JSh_ValueToConstructor(JSContext *cx, jsval* v);

	////////////////////////////////////////////////////////////////////////////////////
	// debugger api
	MOZ_API void JSh_EnableDebugger(/*JSContext* cx, JSObject* global, */const char** src_searchpath, int nums, int port);
	MOZ_API void JSh_UpdateDebugger();
    MOZ_API void JSh_CleanupDebugger();
    // some useful api
    MOZ_API bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file);
    MOZ_API void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file);


    // storeJSObject 将 jsObj 和 nativeObj 表示的类对象存储起来，返回 ID
    //
    OBJID storeJSObject(JS::HandleObject jsObj, JS::HandleObject nativeObj);

    //
    // 根据 nativeObj 查找 ID
    // 
    OBJID jsObj2ID(JS::HandleObject nativeObj);
    JSObject* ID2JSObj(OBJID id);
    bool deleteJSObject(OBJID id);

    MOZ_API void moveVal2Arr(int i, JS::HandleValue val);
    void clearArrObjectVal();
    MOZ_API void removeHeapMapVal(int index);
    MOZ_API int moveVal2HeapMap();
    MOZ_API bool moveValFromMap2Arr(int iMap, int iArr);
    MOZ_API bool addObjectRoot(OBJID id);
    MOZ_API bool removeObjectRoot(OBJID id);
    MOZ_API bool setProperty(OBJID id, const char* name, int iMap);
    MOZ_API void gc();



    enum eGetType
    {
        GetArg = 0,
        GetArgRef = 1,
        GetJSFunRet = 2,
        GetJsval = 3,
    };
    enum eSetType
    {
        SetRval = 0,
        SetArgRef = 1,
        SetJsval = 2,
    };

    typedef bool (* CSEntry)(int op, int slot, int index, bool bStatic, int argc);
    bool JSCall(JSContext *cx, unsigned argc, JS::Value *vp);

    MOZ_API int getCurrIndex();
    MOZ_API void setCurIndex(int i);

    unsigned int argTag(int i);


    extern jschar* getMarshalStringFromJSString(JSContext* cx, JSString* jsStr);
    const jschar* val2String(JS::RootedValue* pval);

    JS::Value& getVpVal();


    MOZ_API short           getChar    (eGetType e);
    MOZ_API char            getSByte   (eGetType e);
    MOZ_API unsigned char   getByte    (eGetType e);
    MOZ_API short           getInt16   (eGetType e);
    MOZ_API unsigned short  getUInt16  (eGetType e);
    MOZ_API int             getInt32   (eGetType e);
    MOZ_API unsigned int    getUInt32  (eGetType e);
    MOZ_API long long       getInt64   (eGetType e);
    MOZ_API unsigned long long getUInt64  (eGetType e);
    MOZ_API int             getEnum    (eGetType e);
    MOZ_API float           getSingle  (eGetType e);
    MOZ_API double          getDouble  (eGetType e);
    MOZ_API long long       getIntPtr  (eGetType e);
    MOZ_API bool getBoolean(eGetType e);
    MOZ_API const jschar* getString(eGetType e);

    MOZ_API void setFloatPtr2(float* f0, float* f1);
    MOZ_API void setFloatPtr3(float* f0, float* f1, float* f2);
    void val2Vector2(JS::RootedValue* pval);
    MOZ_API bool getVector2(eGetType e);
    void val2Vector3(JS::RootedValue* pval);
    MOZ_API bool getVector3(eGetType e);
    MOZ_API OBJID getObject(eGetType e);

    MOZ_API void setChar    (eSetType e, short v);
    MOZ_API void setSByte   (eSetType e, char v);
    MOZ_API void setByte    (eSetType e, unsigned char v);
    MOZ_API void setInt16   (eSetType e, short v);
    MOZ_API void setUInt16  (eSetType e, unsigned short v);
    MOZ_API void setInt32   (eSetType e, int v);
    MOZ_API void setUInt32  (eSetType e, unsigned int v);
    MOZ_API void setInt64   (eSetType e, long long v);
    MOZ_API void setUInt64  (eSetType e, unsigned long long v);
    MOZ_API void setEnum    (eSetType e, int v);
    MOZ_API void setSingle  (eSetType e, float v);
    MOZ_API void setDouble  (eSetType e, double v);
    MOZ_API void setIntPtr  (eSetType e, long long v);
    MOZ_API void setBoolean(eSetType e, bool v);
    MOZ_API void setString(eSetType e, const jschar* value);
    void valSetVector2(JS::RootedValue* pval, float x, float y);
    MOZ_API void setVector2(eSetType e, float x, float y);
    void valSetVector3(JS::RootedValue* pval, float x, float y, float z);
    MOZ_API void setVector3(eSetType e, float x, float y, float z);
    MOZ_API void setObject(eSetType e, OBJID id);
    MOZ_API void setArray(eSetType e, int count);

    MOZ_API bool isVector2(int i);
    MOZ_API bool isVector3(int i);
    MOZ_API void moveTempVal2Arr( int i );

    MOZ_API bool callFunctionName(OBJID jsObjID, const char* functionName, int argCount);

    MOZ_API bool require(JSContext *cx, int argc, JS::Value *vp);
    /////////////////////////////////////////////////////////////////////

    MOZ_API bool evaluate(const char* ascii, size_t length, const char* filename);
    MOZ_API void setRvalBool(jsval* vp, bool v);
    MOZ_API void reportError(const char* err);

    MOZ_API int InitJSEngine(JSErrorReporter er, CSEntry entry, JSNative req);
    MOZ_API void ShutdownJSEngine();
    MOZ_API OBJID NewJSClassObject(char* name);
    MOZ_API bool RemoveJSClassObject(OBJID odjID);
    MOZ_API bool IsJSClassObjectFunctionExist(OBJID objID, const char* functionName);
    MOZ_API JSContext* GetContext();
    MOZ_API JSObject* GetGlobal();
    MOZ_API JSRuntime* GetRuntime();
}

#endif // #ifndef __MOZ_JSWRAP_HEADER__