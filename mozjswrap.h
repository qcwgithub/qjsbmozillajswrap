
#ifndef __MOZ_JSWRAP_HEADER__
#define __MOZ_JSWRAP_HEADER__

#ifdef ENABLE_JS_DEBUG
#pragma message("******** mozjswrap: js debug is enabled! ********") 
#else
#pragma message("******** mozjswrap: js debug is disabled! ********") 
#endif

#ifdef USE_HASH
#include "uthash.h"
#endif
#include "jsapi.h"
#include "mozilla/Maybe.h"
//#include "js/tracer.h"
#include <vector>
#include <map>
#include <list>
#include <string>
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
extern int startMapID;
extern int endMapID;
extern bool GCing;
extern bool g_bUseCacheForStruct;

//typedef void
//(* NurseryCollectCallback)(JSRuntime *rt, JS::gcreason::Reason, int status /* 0 - start 1 - end*/);
//void JS_SetNurseryCollectCallback(NurseryCollectCallback cb);

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

	MOZ_API void getProperty(MAPID id, const char* name);
    MOZ_API void setProperty(MAPID id, const char* name, MAPID valueID);
    MOZ_API void getElement(MAPID id, int i);
    MOZ_API int getArrayLength(MAPID id);

    MOZ_API void gc();
    MOZ_API void MaybeGC();
    MOZ_API int GetGCCount();
    MOZ_API int GetMinorGCCount();

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

    typedef _BOOL (* CSEntry)(
		//int op, int slot, int index, int bStatic, 
		int argsIndex,
		int argc);
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
    MOZ_API void setString(eSetType e, const jschar* value);
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

	MOZ_API void setCallFunctionValueRemoveArr(_BOOL bRemove);
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
	MOZ_API _BOOL evaluate_jsc(const char* ascii, int length, const char* filename);
    MOZ_API const char* getArgString(jsval* vp, int i);
    MOZ_API void setRvalBool(jsval* vp, _BOOL v);
    MOZ_API MAPID getObjFunction(MAPID id, const char* fname);

    MOZ_API int InitJSEngine(JSErrorReporter er, CSEntry entry, JSNative req, OnObjCollected onObjCollected, JSNative print, int option);
    MOZ_API _BOOL initErrorHandler();
    MOZ_API void ShutdownJSEngine(_BOOL bCleanup);
    JSObject* _createJSClassObject(char* name);
    MOZ_API MAPID createJSClassObject(char* name);
    MOZ_API void attachFinalizerObject(MAPID id);
	MOZ_API int newJSClassObject(const char* name);

	MOZ_API int getValueMapSize();
	MOZ_API int getValueMapIndex();
	MOZ_API int getValueMapStartIndex();

	MOZ_API void setUseCacheForStruct(_BOOL b);
	MOZ_API void resetCacheStructIndex();
}


struct AJSCallStack
{
	JS::Value* rval;
	int argc;
	int argIndex;
	int actualArgc;
	JS::CallArgs* args;
	//vector<JS::RootedValue*> lst;

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

struct stHeapValue
{
#ifdef USE_HASH
	int key;
#endif

    JS::Heap<JS::Value> heapValue;
	int refCount;
    bool bTempTrace;

    // bTrace and hasFinalizeOp
    // only one of them could be true
    // or both false
    bool bTrace;
    bool hasFinalizeOp;
	char mark;

#ifdef USE_HASH
	UT_hash_handle hh;
#endif

    stHeapValue()
	{
		Init();
	}
    stHeapValue(JS::HandleValue val) : heapValue(val)
	{
		Init();
	}

	void Init()
	{
		refCount = 0;
		bTrace = false;
		bTempTrace = false;
		hasFinalizeOp = false;
		mark = 0;
	}
};

#ifdef USE_HASH
struct stVMap
{
	uint64_t key;
	int id;
	UT_hash_handle hh;
};
#endif


#ifdef USE_HASH

typedef stHeapValue* VALUEMAP;
typedef stHeapValue* VALUEMAPIT;
typedef stHeapValue* VALUEMAP_VP;

typedef stVMap* VMAP;
typedef stVMap* VMAPIT;

#else // #ifdef USE_HASH

typedef stHeapValue* VALUEMAP_VP;
typedef map<int, stHeapValue > VALUEMAP;
typedef VALUEMAP::iterator VALUEMAPIT;

typedef map<uint64_t, MAPID > VMAP;
typedef VMAP::iterator VMAPIT;

#endif

class valueMap
{
public:
	static VALUEMAP mMap;
	static VMAP VMap;

private:
    static int index;
    static bool tracing;

	static list<int> lstFree;

	static std::list<int> LstTempID;

#ifdef USE_HASH
public:
	static std::list<stHeapValue*> lstHeapValue;
	static stHeapValue* Alloc_stHeapValue()
	{
		stHeapValue* v;
		if (lstHeapValue.size() > 0)
		{
			v = *(--lstHeapValue.end());
			lstHeapValue.pop_back();
		}
		else
		{
			v = new stHeapValue();
		}
		return v;
	}
	static void Return_stHeapValue(stHeapValue* v)
	{
		lstHeapValue.push_back(v);
	}

	static std::list<stVMap*> lststVMap;
	static stVMap* Alloc_stVMap()
	{
		stVMap* v;
		if (lststVMap.size() > 0)
		{
			v = *(--lststVMap.end());
			lststVMap.pop_back();
		}
		else
		{
			v = new stVMap();
		}
		return v;
	}
	static void Return_stVMap(stVMap* v)
	{
		lststVMap.push_back(v);
	}
#endif
public:
	static void clearVMap();
	static void rebuildVMap();
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
	static bool getHasFinalizeOp(MAPID id);
    static bool clear();

	static int getMapSize()
	{
#ifdef USE_HASH
		return (int)HASH_COUNT(mMap);
#else
		return (int)mMap.size();
#endif
	}

	static void _addTempID(MAPID id) { LstTempID.push_back(id); }
	static void _clearTempIDs();
	static int getIndex() { return index; }
};


#ifdef USE_HASH

inline VALUEMAPIT mmap_find(int key)
{
	VALUEMAPIT v;
	HASH_FIND_INT(valueMap::mMap, &key, v);
	return v;
}
#define mit_valid(it) (it != 0)
#define mit_invalid(it) (it == 0)
#define mit_k(it) (it->key)
#define mit_v(it) (*it)
#define mit_pv(it) (it)
#define mmap_erase(it) \
	do \
	{ \
		HASH_DEL(mMap, it); \
	    valueMap::Return_stHeapValue(it); \
	} while(0)

#define mmap_loop(it) for (it = (VALUEMAPIT)mMap; it != 0; it = (VALUEMAPIT)it->hh.next)
#define mmap_newelement(p, J, val, mark) \
	stHeapValue* p = valueMap::Alloc_stHeapValue(); \
	memset(&p->hh, 0, sizeof(p->hh)); \
	p->Init(); \
	p->key = J; \
	p->heapValue.set(val); \
	p->mark = (char)mark

#define mmap_add(k, v) HASH_ADD_INT(mMap, key, v)
#define mmap_clear() HASH_CLEAR(hh, valueMap::mMap)


inline VMAPIT vmap_find(uint64_t key)
{
	VMAPIT v;
	HASH_FIND(hh, valueMap::VMap, &key, sizeof(uint64_t), v);
	return v;
}
#define vit_valid(it) (it != 0)
#define vit_invalid(it) (it == 0)
#define vit_k(it) (it->key)
#define vit_v(it) (it->id)
#define vmap_erase(it) \
	do \
	{ \
		HASH_DEL(VMap, it); \
		valueMap::Return_stVMap(it); \
	} while (0)

#define vmap_add(k, v) \
	do \
	{ \
		stVMap* _p = valueMap::Alloc_stVMap(); \
		memset(&_p->hh, 0, sizeof(_p->hh)); \
		_p->key = k; \
		_p->id = v; \
		HASH_ADD(hh, VMap, key, sizeof(uint64_t), _p); \
	} while (0)

#define vmap_clear() HASH_CLEAR(hh, valueMap::VMap)

#else // #ifdef USE_HASH

#define mmap_find(key) mMap.find(key)
#define mit_valid(it) (it != mMap.end())
#define mit_invalid(it) (it == mMap.end())
#define mit_k(it) (it->first)
#define mit_v(it) (it->second)
#define mit_pv(it) (&(it->second))
#define mmap_erase(it) mMap.erase(it)
#define mmap_loop(it) for (it = mMap.begin(); it != mMap.end(); it++)
#define mmap_newelement(p, J, val, mark) stHeapValue _p(val); \
	_p.mark = (char)mark; \
	stHeapValue* p = &_p
#define mmap_add(k, pv) mMap.insert(VALUEMAP::value_type(k, *pv))
#define mmap_clear() mMap.clear()

#define vmap_find(key) VMap.find(key)
#define vit_valid(it) (it != VMap.end())
#define vit_invalid(it) (it == VMap.end())
#define vit_k(it) (it->first)
#define vit_v(it) (it->second)
#define vmap_erase(it) VMap.erase(it)
#define vmap_add(k, v) VMap.insert(VMAP::value_type(k, v))
#define vmap_clear() VMap.clear()

#endif // #ifdef USE_HASH .. #else

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