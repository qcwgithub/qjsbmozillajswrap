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

void deleteJSObject(JSObject* jsObj)
{
    map<OBJID, MyHeapObj>::iterator it = mapObjs.begin();
    for (; it != mapObjs.end(); it++)
    {
        if (*(it->second.heapJSObj) == jsObj)
        {
            mapObjs.erase(it);
            return;
        }
    }
}