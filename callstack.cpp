#include "callstack.h"
#include "heapObjStorage.h"
//using namespace JS;

//
// global variables
//
//
JS::Value* g_vp = 0;
int g_argc = 0;
int currIndex = 0;

JS::RootedValue* pvalFunRet = 0;
JS::RootedValue* pvalTemp = 0;


bool JSCall(JSContext *cx, int argc, JS::Value *vp)
{
    g_vp = vp;
    g_argc = argc;

    int op = JSVAL_TO_INT(JS_ARGV(cx, vp)[0]);
    int slot = JSVAL_TO_INT(JS_ARGV(cx, vp)[1]);
    int index = JSVAL_TO_INT(JS_ARGV(cx, vp)[2]);
    bool isStatic = JSVAL_TO_BOOLEAN(JS_ARGV(cx, vp)[3]);

    JS::RootedValue valFunRet(g_cx);
    pvalFunRet = &valFunRet;

    JS::RootedValue valTemp(g_cx);
    pvalTemp = &valTemp;

    currIndex = 4;

    return true;
}

int getCurrIndex()
{
    return currIndex;
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
    case GetARGV:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            ret = val2Number<T>(&val);
        }
        break;
    case GetARGVRefOut:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = val2Number<T>(&v);
        }
        break;
    case GetJSFUNRET:
        {
            ret = val2Number<T>(pvalFunRet);
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
float           getSignle  (eGetType e) { return getNumber<float>(e); }
double          getDouble  (eGetType e) { return getNumber<double>(e); }
long long       getIntPtr  (eGetType e) { return getNumber<long long>(e); }

bool getBoolean(eGetType e) {
    bool ret = 0;
    switch (e)
    {
    case GetARGV:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            ret = val.toBoolean();
        }
        break;
    case GetARGVRefOut:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = v.toBoolean();
        }
        break;
    case GetJSFUNRET:
        {
            ret = pvalFunRet->toBoolean();
        }
        break;
    }
    return ret;
}
const jschar* getString(eGetType e) {
    const jschar* ret = 0;
    switch (e)
    {
    case GetARGV:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            ret = val2String(&val);
        }
        break;
    case GetARGVRefOut:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = val2String(&v);
        }
        break;
    case GetJSFUNRET:
        {
            ret = val2String(pvalFunRet);
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
    case GetARGV:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            val2Vector2(&val);
        }
        break;
    case GetARGVRefOut:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            val2Vector2(&v);
        }
        break;
    case GetJSFUNRET:
        {
            val2Vector2(pvalFunRet);
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
    case GetARGV:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            val2Vector3(&val);
        }
        break;
    case GetARGVRefOut:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            val2Vector3(&v);
        }
        break;
    case GetJSFUNRET:
        {
            val2Vector3(pvalFunRet);
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
    case GetARGV:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject obj(g_cx, &val.toObject());
            ret = jsObj2ID(obj);
        }
        break;
    case GetARGVRefOut:
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
    case GetJSFUNRET:
        {
            JS::RootedObject jsObj(g_cx, &pvalFunRet->toObject());
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
        pvalTemp->setInt32(vSet);
        break;
    case SetRval:
        JS_SET_RVAL(g_cx, g_vp, INT_TO_JSVAL(vSet));
        break;
    case UpdateARGVRefOut:
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
        pvalTemp->setDouble(vSet);
        break;
    case SetRval:
        JS_SET_RVAL(g_cx, g_vp, DOUBLE_TO_JSVAL(vSet));
        break;
    case UpdateARGVRefOut:
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
void setSignle  (eSetType e, float v)           { return setNumberF<float>(e, v); }
void setDouble  (eSetType e, double v)          { return setNumberF<double>(e, v); }
void setIntPtr  (eSetType e, long long v)       { return setNumberF<long long>(e, v); }
void setBoolean(eSetType e, bool v)
{
    bool vSet = v;
    switch (e)
    {
    case SetJsval:
        pvalTemp->setBoolean(vSet);
        break;
    case SetRval:
        JS_SET_RVAL(g_cx, g_vp, BOOLEAN_TO_JSVAL(vSet));
        break;
    case UpdateARGVRefOut:
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
    case UpdateARGVRefOut:
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
    case UpdateARGVRefOut:
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
    JS::RootedObject jsObj(g_cx, ID2JSObj(id));
    switch (e)
    {
    case SetJsval:
        pvalTemp->setObjectOrNull(jsObj);
        break;
    case SetRval:
        {
            JS::RootedValue val(g_cx);
            val.setObjectOrNull(jsObj);
            JS_SET_RVAL(g_cx, g_vp, val);
        }
        break;
    case UpdateARGVRefOut:
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
void setArray(eSetType e)
{

}

/////////////////////////////////////////////////////////////////////