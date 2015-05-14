#include "mozjswrap.h"

// objMap
///////////////////////////////////////////////////////////////////////////////////////////////////

objMap::OBJMAP objMap::mMap;
OBJID objMap::lastID = 1; // starts from 1, 0 means nothing

void objMap::trace(JSTracer *trc)
{
//     OBJMAPIT it = mMap.begin();
//     for (; it != mMap.end(); it++)
//     {
//         JS::Heap<JSObject*>* p = &it->second.obj;
//         JS_CallHeapObjectTracer(trc, p, "");
//     }
}

OBJID objMap::add(JS::HandleObject jsObj)
{
    OBJID id = lastID++;
    mMap[id] = stHeapObj(jsObj);
    return id;
}

OBJID objMap::jsObj2ID(JS::HandleObject jsObj, bool autoAdd)
{
	if (jsObj == 0)
	{
		return 0;
	}
    OBJMAPIT it = mMap.begin();
    for (; it != mMap.end(); it++)
    {
        if (it->second.obj == jsObj)
        {
            return it->first;
        }
    }
	if (autoAdd)
	{
		return add(jsObj);
	}
    return 0;
}

JSObject* objMap::id2JSObj(OBJID id)
{
    OBJMAPIT it = mMap.find(id);
    if (it != mMap.end())
    {
        return it->second.obj;
    }
    return 0;
}

bool objMap::remove(OBJID id)
{
    OBJMAPIT it = mMap.find(id);
    if (it != mMap.end())
    {
        mMap.erase(it);
        return true;
    }
    return false;
}

// valueArr
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// for
// 1) Array
// 2) Function Arguments
// 
JS::Heap<JS::Value>* valueArr::arr = 0;
int valueArr::Capacity = 0;
int valueArr::lastIndex = -1;
JS::Heap<JS::Value>* valueArr::makeSureArrHeapObj(int index)
{
    int size = index + 1;
    int& S = Capacity;
    if (arr == 0 || S < size) 
    {
        int oldS = S;
        if (S == 0)
            S = 8;
        while (S < size)
            S *= 2;

        // copy data
        JS::Heap<JS::Value>* _arr = new JS::Heap<JS::Value>[size];
        if (index > 0)
        {
            int N = min(oldS - 1, index);
            for (int i = 0; i < N; i++)
                _arr[i] = arr[i];
        }
        if (arr)
        {
            delete arr;
        }
        arr = _arr;
    }
    return arr;
}

void valueArr::trace(JSTracer *trc)
{
//     if (lastIndex >= 0)
//     {
//         int N = min(lastIndex, Capacity - 1);
//         for (int i = 0; i <= N; i++)
//         {
//             JS_CallHeapValueTracer(trc, &arr[i], "");
//         }
//     }
}

void valueArr::add(int i, JS::HandleValue val)
{
    lastIndex = i;
    JS::Heap<jsval>* arr = makeSureArrHeapObj(i);
    arr[i] = val;
}

void valueArr::clear()
{
    lastIndex = -1;
//     int& index = lastIndex;
//     if (index >= 0)
//     {
//         int N = min(index, Capacity - 1);
//         JS::Value val;
//         val.setUndefined();
//         for (int i = 0; i <= N; i++)
//         {
//             arr[i] = val;
//         }
//         index = -1;
//     }
}

// objRoot
///////////////////////////////////////////////////////////////////////////////////////////////////

std::map<OBJID, JSObject**> objRoot::mapObjectRoot;
bool objRoot::add( OBJID id )
{
    if (mapObjectRoot.find(id) != mapObjectRoot.end())
    {
        return true;
    }
    JSObject* jsObj = objMap::id2JSObj(id);
    if (jsObj == 0)
        return false;
    JSObject** pjsObj = new JSObject*;
    *pjsObj = jsObj;
    bool ret = JS_AddObjectRoot(g_cx, pjsObj);
    mapObjectRoot[id] = pjsObj;
    return ret;
}

bool objRoot::remove( OBJID id )
{
    std::map<OBJID, JSObject**>::iterator it = mapObjectRoot.find(id);
    if (it != mapObjectRoot.end())
    {
        JSObject** pjsObj = it->second;
        JS_RemoveObjectRoot(g_cx, pjsObj);
        delete pjsObj;
        mapObjectRoot.erase(it);
        return true;
    }
    return false;
}

// TODO 所有存在这里的都要立即删除
// （不一定）
valueMap::VALUEMAP valueMap::mMap;
int valueMap::index = 1;

bool valueMap::remove( int i )
{
    VALUEMAPIT it = mMap.find(i);
    if (it != mMap.end())
    {
        delete it->second;
        mMap.erase(it);
        return true;
    }
    return false;
}

void valueMap::trace(JSTracer *trc)
{
//     VALUEMAPIT it = mMap.begin();
//     for (; it != mMap.end(); it++)
//     {
//         JS::Heap<JS::Value>* p = it->second;
//         JS_CallHeapValueTracer(trc, p, "");
//     }
}

int valueMap::add(JS::HandleValue val)
{
    JS::Heap<JS::Value>* p = new JS::Heap<JS::Value>;
    *p = val;
    mMap[index] = p;
    return index++;
}

int valueMap::addFunction(JS::HandleValue val)
{
    JS::RootedValue ns(g_cx);
    if (!JS_ConvertValue(g_cx, val, JSTYPE_FUNCTION, &ns))
        return 0;

    return add(ns);
}

bool valueMap::get(int i, JS::Value* pVal)
{
    VALUEMAPIT it = mMap.find(i);
    if (it != mMap.end())
    {
        *pVal = it->second->get();
        return true;
    }
    return false;
}

bool valueMap::moveFromMap2Arr(int iMap, int iArr)
{
    VALUEMAPIT it = mMap.find(iMap);
    if (it != mMap.end())
    {
        JS::RootedValue val(g_cx, it->second->get());
        valueArr::add(iArr, val);
        return true;
    }
    return false;
}


std::map<int, JS::Value*> valueRoot::mMap;
int valueRoot::index = 1;
int valueRoot::add(JS::HandleValue val)
{
    JS::Value* pVal = new JS::Value;
    *pVal = val;
    bool ret = JS_AddValueRoot(g_cx, pVal);
    mMap[index] = pVal;
    return index++;
}
bool valueRoot::remove(int i)
{
    std::map<int, JS::Value*>::iterator it = mMap.find(i);
    if (it != mMap.end())
    {
        JS::Value* pVal = it->second;
        JS_RemoveValueRoot(g_cx, pVal);
        delete pVal;
        mMap.erase(it);
        return true;
    }
    return false;
}

