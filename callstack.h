#ifndef __callstack_h__
#define __callstack_h__

#include "mozjswrap.h"


enum eGetType
{
    GetARGV = 0,
    GetARGVRefOut = 1,
    GetJSFUNRET = 2,
    Jsval = 3,
};
enum eSetType
{
    SetRval = 0,
    UpdateARGVRefOut = 1,
    SetJsval = 2,
};

extern "C"
{
	typedef bool (* CSEntry)(int op, int slot, int index, bool bStatic, int argc);
	MOZ_API void SetCSEntry(CSEntry entry);
    bool JSCall(JSContext *cx, int argc, JS::Value *vp);

    MOZ_API int getCurrIndex();


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
    MOZ_API float           getSignle  (eGetType e);
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
    MOZ_API void setSignle  (eSetType e, float v);
    MOZ_API void setDouble  (eSetType e, double v);
    MOZ_API void setIntPtr  (eSetType e, long long v);
    MOZ_API void setBoolean(eSetType e, bool v);
    void valSetVector2(JS::RootedValue* pval, float x, float y);
    MOZ_API void setVector2(eSetType e, float x, float y);
    void valSetVector3(JS::RootedValue* pval, float x, float y, float z);
    MOZ_API void setVector3(eSetType e, float x, float y, float z);
    MOZ_API void setObject(eSetType e, OBJID id);


    /////////////////////////////////////////////////////////////////////
}

#endif // #ifndef __callstack_h__