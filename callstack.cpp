#include "mozjswrap.h"


//
// global variables
//
//
MAPID idFunRet = 0; // after callFunctionValue, javascript return value
MAPID idSave = 0; // saving some id to valueMap

JSCallStack jsCallStack;
#define ArgVal(i) (jsCallStack.currStack->args->get(i))
#define ArgIndex (jsCallStack.currStack->argIndex)
#define ActualArgc (jsCallStack.currStack->actualArgc)
#define ArgVp (jsCallStack.currStack->vp)


CSEntry csEntry = 0; 
bool JSCall(JSContext *cx, unsigned argc, JS::Value *vp)
{
	AJSCallStack aStack;
	aStack.vp = vp;
	aStack.argc = argc;
	

	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	aStack.args = &args;

    int op = args.get(0).toInt32();
    int slot = args.get(1).toInt32();
    int index = args.get(2).toInt32();
    bool isStatic = args.get(3).toBoolean();

    //
    // Calculate actual parameter count
    //
    aStack.actualArgc = argc;
    while (aStack.actualArgc > 0 && args.get(aStack.actualArgc - 1).isUndefined())
    {
        aStack.actualArgc--;
	}
	aStack.actualArgc = argc;
    aStack.argIndex = 4;

	jsCallStack.Push(aStack);

	bool ret = (_TRUE == csEntry(op, slot, index, (isStatic ? 1 : 0), aStack.actualArgc - aStack.argIndex));

	valueMap::_clearTempIDs();
	jsCallStack.Pop();
    return ret;
}

int getArgIndex()
{
    return ArgIndex;
}

void setArgIndex(int i)
{
    if (i >= 0 && i < ActualArgc)
    {
        ArgIndex = i;
    }
}

MOZ_API int incArgIndex()
{
    return ArgIndex++;
}

JS::Value getVal(eGetType e, bool bIncIndex)
{
	switch (e)
	{
	case GetArg:
		{
			int i = ArgIndex;
			if (bIncIndex) ArgIndex++;
			return ArgVal(i);
		}
		break;
	case GetArgRef:
		{
			int i = ArgIndex;
			if (bIncIndex) ArgIndex++;
			JS::RootedValue val(g_cx, ArgVal(i));
			JS::RootedObject jsObj(g_cx, &val.toObject());

			JS::RootedValue v(g_cx);
			bool suc = JS_GetProperty(g_cx, jsObj, "Value", &v);
            Assert(suc);
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

			if (bIncIndex) // 2015.Sep.7
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

//extern jschar* getMarshalStringFromJSString(JSContext* cx, JSString* jsStr);
char* val2String(JS::HandleValue val)
{
    JS::RootedString jsStr(g_cx, val.toString());
	if (jsStr == 0)
		return "";
	else
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

_BOOL getBoolean(eGetType e) 
{
	return (getVal(e, true).toBoolean() ? _TRUE : _FALSE);
}
char* getString(eGetType e) 
{
	JS::RootedValue val(g_cx, getVal(e, true));
	return val2String(val);
}
float arrFloat[3];
void getVector2(eGetType e)
{
    JS::RootedValue val(g_cx, getVal(e, true));
    Assert(val.isObject());
    JS::RootedObject obj(g_cx, &val.toObject());

    JS::RootedValue v(g_cx);
    bool suc = false;

    suc = JS_GetProperty(g_cx, obj, "x", &v);
    Assert(suc);
    arrFloat[0] = val2Number<float>(v);

    suc = JS_GetProperty(g_cx, obj, "y", &v);
    Assert(suc);
    arrFloat[1] = val2Number<float>(v);
}
void getVector3(eGetType e)
{
    JS::RootedValue val(g_cx, getVal(e, true));
    Assert(val.isObject());
    JS::RootedObject obj(g_cx, &val.toObject());

    JS::RootedValue v(g_cx);
    bool suc = false;

    suc = JS_GetProperty(g_cx, obj, "x", &v);
    Assert(suc);
    arrFloat[0] = val2Number<float>(v);

    suc = JS_GetProperty(g_cx, obj, "y", &v);
    Assert(suc);
    arrFloat[1] = val2Number<float>(v);

    suc = JS_GetProperty(g_cx, obj, "z", &v);
    Assert(suc);
    arrFloat[2] = val2Number<float>(v);
}
float getObjX()
{
    return arrFloat[0];
}
float getObjY()
{
    return arrFloat[1];
}
float getObjZ()
{
    return arrFloat[2];
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

MOZ_API _BOOL isFunction(eGetType e)
{
	JS::RootedValue val(g_cx, getVal(e, false));
	if (val.isObject())
	{
		return (JS_ObjectIsFunction(g_cx, &val.toObject()) ? _TRUE : _FALSE);
	}
	return _FALSE;
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
            JS_free(g_cx, (void*)str);
            return ret;
		}
	}
	return false;
}

MOZ_API _BOOL isVector2( eGetType e )
{
	JS::RootedValue val(g_cx, getVal(e, false));
	return (valFullNameIs(val, "UnityEngine.Vector2") ? _TRUE : _FALSE);
}

MOZ_API _BOOL isVector3( eGetType e )
{
	JS::RootedValue val(g_cx, getVal(e, false));
	return (valFullNameIs(val, "UnityEngine.Vector3") ? _TRUE : _FALSE);
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
		JS_SET_RVAL(g_cx, ArgVp, val);
		break;
	case SetArgRef:
	default:
		{
			int i = ArgIndex;
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
    JS::RootedValue val(g_cx, INT_TO_JSVAL((int)value));
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
void setBoolean(eSetType e, _BOOL v)
{
    JS::RootedValue val(g_cx, BOOLEAN_TO_JSVAL(v == _TRUE));
	setVal(e, val);
}
void setString(eSetType e, const jschar* value)
{
	JS::RootedString jsString(g_cx, JS_NewUCStringCopyZ(g_cx, value));
    JS::RootedValue val(g_cx, STRING_TO_JSVAL(jsString));
	if (value == 0)
		val.setUndefined();
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

		JS::RootedString jsString(g_cx, JS_NewStringCopyZ(g_cx, "UnityEngine.Vector2"));
		val.setString(jsString); 
		JS_SetProperty(g_cx, jsObj, "_fullname", val);

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

		JS::RootedString jsString(g_cx, JS_NewStringCopyZ(g_cx, "UnityEngine.Vector3"));
		val.setString(jsString); 
		JS_SetProperty(g_cx, jsObj, "_fullname", val);

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
void setArray(eSetType e, int count, _BOOL bClear)
{
    JSObject* _t = JS_NewArrayObject(g_cx, count);
    JS::RootedObject arrObj(g_cx, _t);
    for (int i = 0; i < count; i++)
    {
        JS::RootedValue val(g_cx);
        bool b = valueMap::getVal(valueArr::arr[i], &val);
        Assert(b);
        JS_SetElement(g_cx, arrObj, i, val);
    }
    // clear value array
    valueArr::clear(bClear == _TRUE);

    JS::RootedValue val(g_cx);
	val.setObject(*arrObj);
	setVal(e, val);
}
