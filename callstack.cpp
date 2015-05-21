#include "mozjswrap.h"


//
// global variables
//
//
JS::Value* g_vp = 0;
int g_argc = 0;
// 当前取到第几个参数了
int argIndex = 0;
// 实际几个参数
int actualArgc = 0;

#define ArgVal(i) (JS_ARGV(g_cx, g_vp)[(i)])

MAPID idFunRet; // callFunctionValue后，往 valueMap 添加后得到的IDI
MAPID idSave = 0; //往valueMap添加后得到的ID

CSEntry csEntry = 0; 
bool JSCall(JSContext *cx, unsigned argc, JS::Value *vp)
{
    g_vp = vp;
    g_argc = argc;

    int op = ArgVal(0).toInt32();
    int slot = ArgVal(1).toInt32();
    int index = ArgVal(2).toInt32();
    bool isStatic = ArgVal(3).toBoolean();

    //
    // 计算参数个数，不算末尾的 undefined
    //
    actualArgc = argc;
    while (actualArgc > 0 && ArgVal(actualArgc - 1).isUndefined())
    {
        actualArgc--;
    }

    argIndex = 4;
    bool ret = csEntry(op, slot, index, (isStatic ? 1 : 0), actualArgc - argIndex);

	valueMap::_clearTempIDs();

    return ret;
}

int getArgIndex()
{
    return argIndex;
}

void setArgIndex(int i)
{
    if (i >= 0 && i < actualArgc)
    {
        argIndex = i;
    }
}

MOZ_API int incArgIndex()
{
    return argIndex++;
}

JS::Value getVal(eGetType e, bool bIncIndex)
{
	switch (e)
	{
	case GetArg:
		{
			int i = argIndex;
			if (bIncIndex) argIndex++;
			return ArgVal(i);
		}
		break;
	case GetArgRef:
		{
			int i = argIndex;
			if (bIncIndex) argIndex++;
			JS::RootedValue val(g_cx, ArgVal(i));
			JS::RootedObject jsObj(g_cx, &val.toObject());

			JS::RootedValue v(g_cx);
			JS_GetProperty(g_cx, jsObj, "Value", &v);
			return v;
		}
		break;
    case GetJSFunRet:
        {
            JS::RootedValue val(g_cx);
            valueMap::getVal(idFunRet, &val);
            return val;
		}
		break;
    case GetSaveAndRemove:
    default:
        {
            JS::RootedValue val(g_cx);
            valueMap::getVal(idSave, &val);

            valueMap::removeByID(idSave, false);

            return val;
        }
        break;
	}
}

enum eValueTag
{
    tagUNDEFINED = 1 << 0,
    tagNULL = 1 << 1,
    tagINT32 = 1 << 2,
    tagDOUBLE = 1 << 3,
    tagBOOLEAN = 1 << 4,
    tagSTRING = 1 << 5,
    tagNUMBER = 1 << 6,
    tagOBJECT = 1 << 7,
};
MOZ_API unsigned int getTag(eGetType e)
{
	const JS::Value& val = getVal(e, false);
    unsigned int ret = 0;
    {
        if (val.isUndefined()) ret |= tagUNDEFINED;
        if (val.isNull()) ret |= tagNULL;
        if (val.isInt32()) ret |= tagINT32;
        if (val.isDouble()) ret |= tagDOUBLE;
        if (val.isBoolean()) ret |= tagBOOLEAN;
        if (val.isString()) ret |= tagSTRING;
        if (val.isNumber()) ret |= tagNUMBER;
        if (val.isObject()) ret |= tagOBJECT;
    }
    return ret;
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
const jschar* val2String(JS::HandleValue val)
{
    JSString* jsStr = val.toString();
    return getMarshalStringFromJSString(g_cx, jsStr);
}

template<class T>
T getNumber(eGetType e)
{
	JS::RootedValue val(g_cx, getVal(e, true));
	return val2Number<T>(val);
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

bool getBoolean(eGetType e) 
{
	return getVal(e, true).toBoolean();
}
const jschar* getString(eGetType e) 
{
	JS::RootedValue val(g_cx, getVal(e, true));
	return val2String(val);
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
void val2Vector2(JS::HandleValue pval)
{
    JS::RootedObject obj(g_cx, &pval.toObject());

    JS::RootedValue val(g_cx);

    JS_GetProperty(g_cx, obj, "x", &val);
    *(floatPtr[0]) = val2Number<float>(val);

    JS_GetProperty(g_cx, obj, "y", &val);
    *(floatPtr[1]) = val2Number<float>(val);
}
void val2Vector3(JS::HandleValue pval)
{
    JS::RootedObject obj(g_cx, &pval.toObject());

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
	JS::RootedValue val(g_cx, getVal(e, true));
	val2Vector2(val);
	return true;
}
bool getVector3(eGetType e)
{
	JS::RootedValue val(g_cx, getVal(e, true));
	val2Vector3(val);
	return true;
}
int getObject(eGetType e)
{
	JS::RootedValue val(g_cx, ::getVal(e, true));
	if (val.isObject())
	{
        MAPID id = valueMap::getID(val, true);
		valueMap::_addTempID(id);
		return id;
	}
	return 0;
}

MOZ_API bool isFunction(eGetType e)
{
	JS::RootedValue val(g_cx, getVal(e, false));
	if (val.isObject())
	{
		return JS_ObjectIsFunction(g_cx, &val.toObject());
	}
	return false;
}

bool valFullNameIs(JS::HandleValue val, const char* name)
{
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
			bool ret = (strcmp(str, name) == 0);
            JS_free(g_cx, jsStr);
            return ret;
		}
	}
	return false;
}

MOZ_API bool isVector2( eGetType e )
{
	JS::RootedValue val(g_cx, getVal(e, false));
	return valFullNameIs(val, "UnityEngine.Vector2");
}

MOZ_API bool isVector3( eGetType e )
{
	JS::RootedValue val(g_cx, getVal(e, false));
	return valFullNameIs(val, "UnityEngine.Vector3");
}

MOZ_API int getFunction(eGetType e)
{
	JS::RootedValue val(g_cx, getVal(e, true));
	return valueMap::addFunction(val);
}

void setVal(eSetType e, JS::HandleValue val)
{
	switch (e)
	{
	case SetSaveAndTempTrace:
        {
            idSave = valueMap::add(val, 7);
            valueMap::setTempTrace(idSave, true);
        }
		break;
	case SetRval:
		JS_SET_RVAL(g_cx, g_vp, val);
		break;
	case SetArgRef:
	default:
		{
			int i = argIndex;
			JS::RootedValue v(g_cx, ArgVal(i));
			JS::RootedObject jsObj(g_cx, &v.toObject());

			JS::RootedValue mfval(g_cx, val);
			JS_SetProperty(g_cx, jsObj, "Value", mfval);
		}
		break;
	}
}

template<class T>
void setNumberI(eSetType e, T value)
{
    JS::RootedValue val(g_cx, INT_TO_JSVAL((double)value));
	setVal(e, val);
}

template<class T>
void setNumberF(eSetType e, T value)
{
    JS::RootedValue val(g_cx, DOUBLE_TO_JSVAL((double)value));
	setVal(e, val);
}
void setUndefined(eSetType e)
{
	JS::RootedValue val(g_cx);
	val.setUndefined();
	setVal(e, val);
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
    JS::RootedValue val(g_cx, BOOLEAN_TO_JSVAL(v));
	setVal(e, val);
}
void setString(eSetType e, const jschar* value)
{
	// TODO fix memory leak
	JS::RootedString jsString(g_cx, JS_NewUCStringCopyZ(g_cx, value));
    JS::RootedValue val(g_cx, STRING_TO_JSVAL(jsString));
	setVal(e, val);
}
void setVector2(eSetType e, float x, float y)
{
    JS::RootedObject jsObj(g_cx, _createJSClassObject("UnityEngine.Vector2"));
    if (jsObj)
    {
        JS::RootedValue val(g_cx);

        val.setDouble(x); JS_SetProperty(g_cx, jsObj, "x", val);
        val.setDouble(y); JS_SetProperty(g_cx, jsObj, "y", val);

        val.setObject(*jsObj);
        setVal(e, val);
    }
}
void setVector3(eSetType e, float x, float y, float z)
{
    JS::RootedObject jsObj(g_cx, _createJSClassObject("UnityEngine.Vector3"));
    if (jsObj)
    {
        JS::RootedValue val(g_cx);

        val.setDouble(x); JS_SetProperty(g_cx, jsObj, "x", val);
        val.setDouble(y); JS_SetProperty(g_cx, jsObj, "y", val);
        val.setDouble(z); JS_SetProperty(g_cx, jsObj, "z", val);

        val.setObject(*jsObj);
        setVal(e, val);
    }
}
void setObject(eSetType e, int id)
{
    JS::RootedValue val(g_cx);
    if (id == 0 || !valueMap::getVal(id, &val))
    {
        val.setUndefined();
    }

	setVal(e, val);
}
void setFunction(eSetType e, int funID)
{
    setObject(e, funID);
//     JS::RootedValue val(g_cx);
//     if (!valueMap::getVal(funID, &val))
//     {
//         val.setUndefined();
//     }
// 
// 	setVal(e, _v);
}
void setArray(eSetType e, int count, bool bClear)
{
    JSObject* _t = JS_NewArrayObject(g_cx, count, 0 /* jsval* */);
    JS::RootedObject arrObj(g_cx, _t);
    for (int i = 0; i < count; i++)
    {
        JS::RootedValue val(g_cx);
        bool b = valueMap::getVal(valueArr::arr[i], &val);
        Assert(b);
        JS_SetElement(g_cx, arrObj, i, &val);
    }
    // clear value array
    valueArr::clear(bClear);

    JS::RootedValue val(g_cx);
	val.setObject(*arrObj);
	setVal(e, val);
}
