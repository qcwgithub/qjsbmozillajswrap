
#ifndef __MOZ_JSWRAP_HEADER__
#define __MOZ_JSWRAP_HEADER__

#ifdef ENABLE_JS_DEBUG
#pragma message("******** mozjswrap: js debug is enabled! ********") 
#else
#pragma message("******** mozjswrap: js debug is disabled! ********") 
#endif

#include "jsapi.h"
#include <vector>
#include <map>
using namespace std;

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
typedef int FUNCTIONID;

extern JS::Heap<JS::Value> valFunRet;
extern JS::Heap<JS::Value> valTemp;
extern JSObject** ppCSObj;
extern FUNCTIONID jsErrorEntry;

struct stHeapObj
{
    JS::Heap<JSObject*>* heapJSObj;
    //JSObject* jsObj; // old obj, just remember here
};

extern "C"
{
	/*
    * Error handler
    */
	MOZ_API int getErroReportLineNo(JSErrorReport* report);
    MOZ_API const char* getErroReportFileName(JSErrorReport* report);
    MOZ_API void reportError(const char* err);

	/*
    * Debugger
    */
	MOZ_API void enableDebugger(/*JSContext* cx, JSObject* global, */const char** src_searchpath, int nums, int port);
	MOZ_API void updateDebugger();
    MOZ_API void cleanupDebugger();
    MOZ_API bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file);
    MOZ_API void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file);


    // 第1个参数是个id
    // 因为会调用 setProperty 前面一定是用创建 JS 类的对象
    MOZ_API bool setProperty(OBJID id, const char* name, int iMap);
    MOZ_API bool getElement(OBJID id, int i);
    MOZ_API int getArrayLength(OBJID id);

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
	typedef void (* OnObjCollected)(OBJID id);
    bool JSCall(JSContext *cx, unsigned argc, JS::Value *vp);

    MOZ_API int getArgIndex();
    MOZ_API void setArgIndex(int i);
    MOZ_API int incArgIndex();



    extern jschar* getMarshalStringFromJSString(JSContext* cx, JSString* jsStr);
    const jschar* val2String(JS::HandleValue pval);

	MOZ_API unsigned int getTag(eGetType e);
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
    void val2Vector2(JS::HandleValue pval);
    MOZ_API bool getVector2(eGetType e);
    void val2Vector3(JS::HandleValue pval);
    MOZ_API bool getVector3(eGetType e);
    MOZ_API OBJID getObject(eGetType e);
    MOZ_API bool isFunction(eGetType e);
    MOZ_API int getFunction(eGetType e);

    MOZ_API void setUndefined(eSetType e);
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
    void valSetVector2(JS::Value* pval, float x, float y);
    MOZ_API void setVector2(eSetType e, float x, float y);
    void valSetVector3(JS::Value* pval, float x, float y, float z);
    MOZ_API void setVector3(eSetType e, float x, float y, float z);
    MOZ_API void setObject(eSetType e, OBJID id);
    MOZ_API void setArray(eSetType e, int count);
    MOZ_API void setFunction(eSetType e, int funID);

    MOZ_API bool isVector2(eGetType e);
    MOZ_API bool isVector3(eGetType e);

    // val movement
    MOZ_API void moveTempVal2Arr( int i );
    MOZ_API void moveTempVal2Map( );
    MOZ_API void removeValFromMap( int i );
    MOZ_API bool moveValFromMap2Arr(int iMap, int iArr);

    MOZ_API bool callFunctionValue(OBJID jsObjID, int funID, int argCount);
    MOZ_API bool addObjectRoot(int id);
    MOZ_API bool removeObjectRoot(int id);
    MOZ_API bool addValueRoot(int id);
    MOZ_API bool removeValueRoot(int id);

    // 目前仅用于 arg，不够用的话要再加 (GetType e) 参数
    MOZ_API OBJID addArgObj2Map();
    MOZ_API void removeObjFromMap(OBJID id);

    //MOZ_API bool require(JSContext *cx, int argc, JS::Value *vp);
    /////////////////////////////////////////////////////////////////////

    MOZ_API bool evaluate(const char* ascii, size_t length, const char* filename);
    MOZ_API const jschar* getArgString(jsval* vp, int i);
    MOZ_API void setRvalBool(jsval* vp, bool v);
    MOZ_API FUNCTIONID getObjFunction(OBJID id, const char* fname);

    MOZ_API int InitJSEngine(JSErrorReporter er, CSEntry entry, JSNative req, OnObjCollected onObjCollected);
    MOZ_API bool initErrorHandler();
    MOZ_API void ShutdownJSEngine();
    MOZ_API OBJID createJSClassObject(char* name);
    MOZ_API bool attachFinalizerObject(OBJID id);
    MOZ_API OBJID newJSClassObject(const jschar* name);
    MOZ_API bool RemoveJSClassObject(OBJID odjID);
}

extern CSEntry csEntry;

class objMap
{
    static map<OBJID, stHeapObj> mMap;
    static OBJID lastID;

public:
    static OBJID add(JS::HandleObject jsObj);
    static bool remove(OBJID id);
    static OBJID jsObj2ID(JS::HandleObject jsObj, bool autoAdd = false);
    static JSObject* id2JSObj(OBJID id);
};

class valueArr
{
public:
    static JS::Heap<JS::Value>* arr;
private:
    static int size;
    static int lastIndex;
    static JS::Heap<JS::Value>* makeSureArrHeapObj(int index);

public:
    static void add(int i, JS::HandleValue val);
    static void clear();
};

class valueMap
{
    // TODO 所有存在这里的都要立即删除
    // （不一定）
    static std::map<int, JS::Heap<JS::Value>* > mMap;
    static int index;
public:
    static int add(JS::HandleValue val);
    static int addFunction(JS::HandleValue val);
    static bool get(int i, JS::Value* pVal);
    static void remove( int index );
    static bool moveFromMap2Arr(int iMap, int iArr);
};

class objRoot
{
    static std::map<OBJID, JSObject**> mapObjectRoot;
public:
    static bool add( OBJID id );
    static bool remove( OBJID id );
};

class valueRoot
{
    static std::map<int, JS::Value*> mMap;
    static int index;
public:
    static int add(JS::HandleValue val);
    static bool remove(int i);
};

// class argUtil
// {
// public:
//     void setVp(jsval* val) 
//     {
//         this->val = val;
//     }
// 
//     jsval* val;
// };

#endif // #ifndef __MOZ_JSWRAP_HEADER__