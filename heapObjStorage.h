#ifndef __heap_obj_storage_h__
#define __heap_obj_storage_h__

#include "mozjswrap.h"

//
// storeJSObject 将 jsObj 和 nativeObj 表示的类对象存储起来，返回 ID
//
OBJID storeJSObject(JS::HandleObject jsObj, JS::HandleObject nativeObj);

//
// 根据 nativeObj 查找 ID
// 
OBJID jsObj2ID(JS::HandleObject nativeObj);
JSObject* ID2JSObj(OBJID id);
bool deleteJSObject(OBJID id);

MOZ_API void moveVal2Arr(int i, JS::HandleValue val);
void clearArrObjectVal();
MOZ_API void removeHeapMapVal(int index);
MOZ_API int moveVal2HeapMap();
MOZ_API bool moveValFromMap2Arr(int iMap, int iArr);
MOZ_API bool addObjectRoot(OBJID id);
MOZ_API bool removeObjectRoot(OBJID id);

#endif // #ifndef __heap_obj_storage_h__