#include "mozjswrap.h"


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
bool JSCall(JSContext *cx, unsigned argc, JS::Value *vp)
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
T val2Number(JS::HandleValue val)
{
    if (val.isDouble())
        return (T)val.toDouble();
    else
        return (T)val.toInt32();
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
            ret = val2Number<T>(val);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);
            ret = val2Number<T>(v);
        }
        break;
    case GetJSFunRet:
        {
            JS::RootedValue val(g_cx, valFunRet);
            ret = val2Number<T>(val);
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
            JS::RootedValue val(g_cx, valFunRet);
            ret = val2String(&val);
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
    *(floatPtr[0]) = val2Number<float>(val);

    JS_GetProperty(g_cx, obj, "y", &val);
    *(floatPtr[1]) = val2Number<float>(val);
}
void val2Vector3(JS::RootedValue* pval)
{
    JS::RootedObject obj(g_cx, &pval->toObject());

    JS::RootedValue val(g_cx);

    JS_GetProperty(g_cx, obj, "x", &val);
    *(floatPtr[0]) = val2Number<float>(val);

    JS_GetProperty(g_cx, obj, "y", &val);
    *(floatPtr[1]) = val2Number<float>(val);

    JS_GetProperty(g_cx, obj, "z", &val);
    *(floatPtr[2]) = val2Number<float>(val);
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
            JS::RootedValue val(g_cx, valFunRet);
            val2Vector2(&val);
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
            JS::RootedValue val(g_cx, valFunRet);
            val2Vector3(&val);
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
            ret = objMap::jsObj2ID(obj);
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
            ret = objMap::jsObj2ID(jsObj2);
        }
        break;
    case GetJSFunRet:
        {
            JS::RootedObject jsObj(g_cx, &valFunRet.toObject());
            ret = objMap::jsObj2ID(jsObj);
        }
        break;
    }
    return ret;
}

MOZ_API bool isFunction(eGetType e)
{
    bool ret = false;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex; // no ++
            JS::RootedValue val(g_cx, g_vp[i]);

            //val->toString()
            //  val->is
            //JSVAL_TO_DOUBLE()

            if (val.isObject())
            {
                ret = JS_ObjectIsFunction(g_cx, &val.toObject());
            }
        }
        break;
    case GetArgRef:
        {
            int i = currIndex; // no ++
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            if (v.isObject())
            {
                ret = JS_ObjectIsFunction(g_cx, &v.toObject());
            }
        }
        break;
    case GetJSFunRet:
        {
            if (valFunRet.isObject())
            {
                ret = JS_ObjectIsFunction(g_cx, &valFunRet.toObject());
            }
        }
        break;
    }
    return ret;
}

MOZ_API int getFunction(eGetType e)
{
    OBJID ret = 0;
    switch (e)
    {
    case GetArg:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);

            ret = valueMap::addFunction(val);
        }
        break;
    case GetArgRef:
        {
            int i = currIndex++;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS_GetProperty(g_cx, jsObj, "Value", &v);

            ret = valueMap::addFunction(v);
        }
        break;
    case GetJSFunRet:
        {
            ret = valueMap::addFunction(valFunRet);
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
void setUndefined(eSetType e)
{
    switch (e)
    {
    case SetJsval:
        valTemp.setUndefined();
        break;
    case SetRval:
        {
            JS::Value val;
            val.setUndefined();
            JS_SET_RVAL(g_cx, g_vp, val);
        }
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            v.setUndefined();
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
        {
            JS::RootedString jsString(g_cx, JS_NewUCStringCopyZ(g_cx, value));
            valTemp.setString(jsString);
        }
        break;
    case SetRval:
        {
            JS::RootedString jsString(g_cx, JS_NewUCStringCopyZ(g_cx, value));
            JS_SET_RVAL(g_cx, g_vp, STRING_TO_JSVAL(jsString));
        }
        break;
    case SetArgRef:
        {
            int i = currIndex;
            JS::RootedValue val(g_cx, g_vp[i]);
            JS::RootedObject jsObj(g_cx, &val.toObject());

            JS::RootedValue v(g_cx);
            JS::RootedString jsString(g_cx, JS_NewUCStringCopyZ(g_cx, value));
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
        {
            JS::RootedValue val(g_cx, valTemp);
            valSetVector2(&val, x, y);
        }
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
        {
            JS::RootedValue val(g_cx, valTemp);
            valSetVector3(&val, x, y, z);
        }
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
    JS::RootedObject jsObj(g_cx, objMap::id2JSObj(id));
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
        JS::RootedValue val(g_cx, valueArr::arr[i]);
        JS_SetElement(g_cx, arrObj, i, &val);
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
    JS::RootedValue val(g_cx, JS_ARGV(g_cx, g_vp)[i]);
    if (val.isObject())
    {
        JS::RootedObject obj(g_cx, &val.toObject());
        JS::RootedValue v(g_cx);
        JS_GetProperty(g_cx, obj, "_fullname", &v);
        if (v.isString())
        {
            // TODO fix memory leak
            JS::RootedString jsStr(g_cx, v.toString());
            const char* str = JS_EncodeStringToUTF8(g_cx, jsStr);
            return strcmp(str, "UnityEngine.Vector2") == 0;
        }
    }
    return false;
}

MOZ_API bool isVector3( int i )
{
    JS::RootedValue val(g_cx, JS_ARGV(g_cx, g_vp)[i]);
    if (val.isObject())
    {
        JS::RootedObject obj(g_cx, &val.toObject());
        JS::RootedValue v(g_cx);
        JS_GetProperty(g_cx, obj, "_fullname", &v);
        if (v.isString())
        {
            // TODO fix memory leak
            JS::RootedString jsStr(g_cx,  v.toString());
            const char* str = JS_EncodeStringToUTF8(g_cx, jsStr);
            return strcmp(str, "UnityEngine.Vector3") == 0;
        }
    }
    return false;
}