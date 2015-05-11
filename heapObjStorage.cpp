#include "heapObjStorage.h"

#include <map>
using namespace std;

map<OBJID, MyHeapObj> mapObjs;
OBJID objID = 1; // starts from 1, 0 means nothing

OBJID storeJSObject(JS::HandleObject jsObj, JS::HandleObject nativeObj)
{
    MyHeapObj st;
    {
        st.heapJSObj = new JS::Heap<JSObject*>(jsObj);
        st.jsObj = jsObj;
        st.heapNativeObj = new JS::Heap<JSObject*>(nativeObj);
        st.nativeObj = nativeObj;
    }
    OBJID id = objID++;
    mapObjs[id] = st;
    return id;
}

OBJID jsObj2ID(JS::HandleObject nativeObj)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.begin();
    for (; it != mapObjs.end(); it++)
    {
        if (*(it->second.heapNativeObj) == nativeObj)
        {
            return it->first;
        }
    }
    return 0;
}

JSObject* ID2JSObj(OBJID id)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.find(id);
    if (it != mapObjs.end())
        return *(it->second.heapJSObj);
    return 0;
}

bool deleteJSObject(OBJID id)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.find(id);
    if (it != mapObjs.end())
        mapObjs.erase(it);
//     map<OBJID, MyHeapObj>::iterator it = mapObjs.begin();
//     for (; it != mapObjs.end(); it++)
//     {
//         if (*(it->second.heapJSObj) == jsObj)
//         {
// 			delete it->second.heapJSObj;
// 			delete it->second.heapNativeObj;
//             mapObjs.erase(it);
//             return;
//         }
//     }
}

//
// for
// 1) Array
// 2) Function Arguments
// 
JS::Heap<JS::Value>* arrHeapObj = 0;
int arrHeapObjSize = 0;
int arrHeapObjLastIndex = -1;
JS::Heap<JS::Value>* makeSureArrHeapObj(int index)
{
    int size = index + 1;
    int& S = arrHeapObjSize;
    if (arrHeapObj == 0 || S < size) 
    {
        int oldS = S;
        if (S == 0)
            S = 8;
        while (S < size)
            S *= 2;

        JS::Heap<JS::Value>* arr = new JS::Heap<JS::Value>[arrHeapObjSize];
        if (index > 0)
        {
            int N = min(oldS - 1, index);
            for (int i = 0; i < N; i++)
                arr[i] = oldS[i];
        }
        arrHeapObj = arr;
    }
}

MOZ_API void moveVal2Arr(int i, JS::HandleValue val)
{
    JS::Heap<JS::Value>* arr = makeSureArrHeapObj(i);
    arr[i] = val;
	arrHeapObjLastIndex = i;
}

void clearArrObjectVal()
{
	int& index = arrHeapObjLastIndex;
	if (index >= 0)
	{
		for (int i = 0; i <= index; i++)
		{
			arrHeapObj[i] = 0;
		}
		index = -1;
	}
}

std::map<OBJID, JSObject**> mapObjectRoot;
MOZ_API bool addObjectRoot( OBJID id )
{
	if (mapObjectRoot.find(id) != mapObjectRoot.end())
	{
		return true;
	}
	JSObject* jsObj = ID2JSObj(id);
	if (jsObj == 0)
		return false;
	JSObject** pjsObj = new JSObject*;
	*pjsObj = jsObj;
	bool ret = JS_AddObjectRoot(g_cx, pjsObj);
	mapObjectRoot[id] = pjsObj;
	return ret;
}

MOZ_API bool removeObjectRoot( OBJID id )
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

std::map<int, JS::Heap<JS::Value> > mapHeapVal;
int mapHeapVal_i = 1;

MOZ_API void removeHeapMapVal( int index )
{
	std::map<int, JS::Heap<JS::Value> >::iterator it = mapHeapVal.find(index);
	if (it != mapHeapVal.end())
		mapHeapVal.erase(it);
}

MOZ_API int moveVal2HeapMap()
{
	mapHeapVal[mapHeapVal_i] = valTemp;
	return mapHeapVal_i++;
}

bool moveValFromMap2Arr(int iMap, int iArr)
{
	std::map<int, JS::Heap<JS::Value> >::iterator it = mapHeapVal.find(iMap);
	if (it != mapHeapVal.end())
	{
		JS::RootedValue val(g_cx, it->second.get());
		moveVal2Arr(iArr, val);
		return true;
	}
	return false;
}