
#ifndef __MOZ_JSWRAP_HEADER__
#define __MOZ_JSWRAP_HEADER__

#ifdef ENABLE_JS_DEBUG
#pragma message("******** mozjswrap: js debug is enabled! ********") 
#else
#pragma message("******** mozjswrap: js debug is disabled! ********") 
#endif

#include "jsapi.h"
#include "mozilla/Maybe.h"
//#include "js/tracer.h"
#include <vector>
#include <map>
#include <list>
using namespace std;

#ifdef _WINDOWS

// 1
#if defined(MOZ_JSWRAP_COMPILATION)
#define MOZ_API __declspec( dllexport ) 
#else
#define MOZ_API __declspec( dllimport )
#endif

// 2
#include <assert.h>
#define Assert assert
#else
#define Assert
#define MOZ_API
#endif

extern JSRuntime* g_rt;
extern JSContext* g_cx;
extern mozilla::Maybe<JS::PersistentRootedObject> g_global;

typedef int MAPID;

extern MAPID idFunRet; // callFunctionValue后，往 valueMap 添加后得到的IDI
extern MAPID idSave; //往valueMap添加后得到的ID

extern JS::Heap<JSObject*>* ppCSObj;
extern MAPID idErrorEntry;
extern bool shutingDown;

// marshal
// it seems they treat bool as int
#define _BOOL int
#define _FALSE 0
#define _TRUE 1


#ifndef JS_ARGV
#define JS_ARGV(cx,vp)          ((vp) + 2)
#endif


#ifndef JS_SET_RVAL
#define JS_SET_RVAL(cx,vp,v)    (*(vp) = (v))
#endif


char* getMarshalStringFromJSString(JSContext* cx, JS::HandleString jsStr);
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
//     MOZ_API bool Jsh_RunScript(JSContext* cx, JSObject* global, const char* script_file);
//     MOZ_API void Jsh_CompileScript(JSContext* cx, JSObject* global, const char* script_file);

    MOZ_API void setProperty(MAPID id, const char* name, MAPID valueID);
    MOZ_API void getElement(MAPID id, int i);
    MOZ_API int getArrayLength(MAPID id);

    MOZ_API void gc();

    enum eGetType
    {
        GetArg = 0,
        GetArgRef = 1,
        GetJSFunRet = 2,
        GetSaveAndRemove = 3,
    };
    enum eSetType
    {
        SetRval = 0,
        SetArgRef = 1,
        SetSaveAndTempTrace = 2,
    };

    typedef _BOOL (* CSEntry)(int op, int slot, int index, int bStatic, int argc);
	typedef void (* OnObjCollected)(MAPID id);
    bool JSCall(JSContext *cx, unsigned argc, JS::Value *vp);

    MOZ_API int getArgIndex();
    MOZ_API void setArgIndex(int i);
    MOZ_API int incArgIndex();

    char* val2String(JS::HandleValue pval);

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
    MOZ_API _BOOL getBoolean(eGetType e);
    MOZ_API char* getString(eGetType e);

    MOZ_API void getVector2(eGetType e);
    MOZ_API void getVector3(eGetType e);
    MOZ_API float getObjX();
    MOZ_API float getObjY();
    MOZ_API float getObjZ();

    MOZ_API int getObject(eGetType e);
    MOZ_API _BOOL isFunction(eGetType e);
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
    MOZ_API void setBoolean(eSetType e, _BOOL v);
    MOZ_API void setString(eSetType e, const char* value);
    MOZ_API void setVector2(eSetType e, float x, float y);
    MOZ_API void setVector3(eSetType e, float x, float y, float z);
    MOZ_API void setObject(eSetType e, int id);
    MOZ_API void setArray(eSetType e, int count, _BOOL bClear);
    MOZ_API void setFunction(eSetType e, int funID);

    MOZ_API _BOOL isVector2(eGetType e);
    MOZ_API _BOOL isVector3(eGetType e);

    // val movement
    MOZ_API void moveSaveID2Arr( int arrIndex );
    MOZ_API MAPID getSaveID( );
    MOZ_API void removeByID( MAPID id );
    MOZ_API void moveID2Arr(int id, int arrIndex);

    MOZ_API void callFunctionValue(MAPID jsObjID, MAPID funID, int argCount);
	MOZ_API int incRefCount(MAPID id);
	MOZ_API int decRefCount(MAPID id);
	MOZ_API void setTrace(MAPID id, _BOOL bTrace);
    MOZ_API _BOOL isTraced(MAPID id);
    MOZ_API void setTempTrace(MAPID id, _BOOL bTempTrace);

    MOZ_API MAPID addArgObj2Map();
    MOZ_API void removeObjFromMap(MAPID id);

    //MOZ_API bool require(JSContext *cx, int argc, JS::Value *vp);
    /////////////////////////////////////////////////////////////////////

    MOZ_API _BOOL evaluate(const char* ascii, int length, const char* filename);
    MOZ_API const char* getArgString(jsval* vp, int i);
    MOZ_API void setRvalBool(jsval* vp, _BOOL v);
    MOZ_API MAPID getObjFunction(MAPID id, const char* fname);

    MOZ_API int InitJSEngine(JSErrorReporter er, CSEntry entry, JSNative req, OnObjCollected onObjCollected, JSNative print);
    MOZ_API _BOOL initErrorHandler();
    MOZ_API void ShutdownJSEngine(_BOOL bCleanup);
    JSObject* _createJSClassObject(char* name);
    MOZ_API MAPID createJSClassObject(char* name);
    MOZ_API void attachFinalizerObject(MAPID id);
	MOZ_API int newJSClassObject(const char* name);

	MOZ_API int getValueMapSize();
	MOZ_API int getValueMapIndex();


}


struct AJSCallStack
{
	JS::Value* vp;
	int argc;
	int argIndex;
	int actualArgc;
	JS::CallArgs* args;

	//	MAPID idFunRet;
	//	MAPID idSave;
};

struct JSCallStack
{
	AJSCallStack* currStack;
	std::list<AJSCallStack> lstStack;
	void Push(AJSCallStack a)
	{
		lstStack.push_back(a);
		currStack = &(*(--lstStack.end()));
	}
	void Pop()
	{
		Assert(lstStack.size() > 0);
		lstStack.erase(--lstStack.end());

		if (lstStack.size() > 0)
			currStack = &(*(--lstStack.end()));
		else
			currStack = 0;
	}
};
extern JSCallStack jsCallStack;


extern CSEntry csEntry;

// struct stHeapObj
// {
//     JS::Heap<JSObject*> obj;
//     stHeapObj(){}
//     stHeapObj(JS::HandleObject o){ obj = o; }
// };

//typedef JS::Heap<JSObject*> stHeapObj;

// class objMap
// {
//     typedef map<OBJID, stHeapObj > OBJMAP;
//     typedef OBJMAP::iterator OBJMAPIT;
// 
//     static OBJMAP mMap;
//     static OBJID lastID;
// 
// public:
//     static void trace(JSTracer *trc);
//     static OBJID add(JS::HandleObject jsObj);
//     static bool remove(OBJID id);
//     static OBJID jsObj2ID(JS::HandleObject jsObj, bool autoAdd = false);
//     static JSObject* id2JSObj(OBJID id);
// };

class valueArr
{
public:
    static int* arr;
private:
    static int Capacity;
    static int lastIndex;
    static int* makeSureArrHeapObj(int index);

public:
    static void add(int arrIndex, MAPID iMap);
    static void clear(bool bClear);
};


// struct stHeapValue
// {
//     JS::Heap<JS::Value> val;
//     stHeapValue(){}
//     stHeapValue(JS::HandleValue v){ val = v; }
// };

struct stHeapValue
{
    JS::Heap<JS::Value> heapValue;
	int refCount;
    bool bTempTrace;

    // bTrace and hasFinalizeOp
    // only one of them could be true
    // or both false
    bool bTrace;
    bool hasFinalizeOp;
	char mark;
    stHeapValue() : refCount(0), bTrace(false), bTempTrace(false), hasFinalizeOp(false), mark(0) {}
    stHeapValue(JS::HandleValue val) : refCount(0), bTrace(false), bTempTrace(false), hasFinalizeOp(false), heapValue(val), mark(0) {}
};

// get object, ignore failure
#define MGETOBJ0(id, obj) \
    JS::RootedObject obj(g_cx, 0);\
    JS::RootedValue ___objVal(g_cx);\
    if (valueMap::getVal(id, &___objVal) && ___objVal.isObject())\
    {\
        obj = &___objVal.toObject();\
    }

// get object, if fail, return false
#define MGETOBJ1(id, obj, er) \
    JS::RootedValue ___valObj(g_cx);\
    if (!valueMap::getVal(id, &___valObj) ||\
        !___valObj.isObject())\
    {\
        Assert(false, "MGETOBJ1 fail");\
        return er;\
    }\
    JS::RootedObject obj(g_cx, &___valObj.toObject());

/*



*/

class valueMap
{
	// SpiderMonkey'GC is moving GC
	// which means object's address may change after GC
	// so we put object's jsval in stHeapValue structure to make sure always having newest address
    typedef map<int, stHeapValue > VALUEMAP;
    typedef VALUEMAP::iterator VALUEMAPIT;
    static VALUEMAP mMap;
    static int index;
    static bool tracing;

    typedef map<uint64_t, MAPID > VMAP;
	static VMAP VMap;
	static list<int> lstFree;

	static std::list<int> LstTempID;

public:
    static MAPID add(JS::HandleValue val, int mark);
    static void trace(JSTracer *trc);
    static MAPID addFunction(JS::HandleValue val);
    static bool getVal(MAPID i, JS::MutableHandleValue pVal);
    static MAPID getID(const JS::Value& val, bool autoAdd);
    static bool removeByID( MAPID i, bool bForce );
    static MAPID containsValue(JS::Value v);
	static int incRefCount(MAPID id);
	static int decRefCount(MAPID id);
	static bool isTraced(MAPID id);
    static bool setTrace(MAPID id, bool trace);
    static bool setTempTrace(MAPID id, bool tempTrace);
    static bool setHasFinalizeOp(MAPID id, bool has);
    static bool clear();

	static int getMapSize(){ return (int)mMap.size(); }

	static void _addTempID(MAPID id) { LstTempID.push_back(id); }
	static void _clearTempIDs();
	static int getIndex() { return index; }
};

// class objRoot
// {
//     static std::map<OBJID, JSObject**> mapObjectRoot;
// public:
//     static bool add( OBJID id );
//     static bool remove( OBJID id );
//     static bool containsValue(JSObject* obj);
// };

// class valueRoot
// {
//     static std::map<int, JS::Value*> mMap;
//     static int index;
// public:
//     static int add(JS::HandleValue val);
//     static bool remove(int i);
// };

template<class T>
class variableLengthArray
{
    T* arr;
    int length;
public:
    variableLengthArray() : arr(0), length(0){}
    T* get(int len)
    {
        // no copy
        if (arr == 0 || this->length < len)
        {
            if (arr) delete arr;
            arr = new T[len];
            this->length = len;
        }
        return arr;
    }
};

struct SplitUtil 
{
    static variableLengthArray<char> sArr;
    SplitUtil(char* s, const char* sp) 
    { 
        size_t len = strlen(s);
        char* _s = sArr.get(len + 1);
        memcpy(_s, s, len);
        _s[len] = 0;

        str = _s; 
        splitStr = sp;
    }
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