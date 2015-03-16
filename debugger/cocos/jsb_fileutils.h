
#pragma  once

#include "jsapi.h"
#include "jsfriendapi.h"

extern JSClass  *jsb_cocos2d_FileUtils_class;
extern JSObject *jsb_cocos2d_FileUtils_prototype;

bool js_cocos2dx_FileUtils_constructor(JSContext *cx, uint32_t argc, jsval *vp);
void js_cocos2dx_FileUtils_finalize(JSContext *cx, JSObject *obj);
void js_register_cocos2dx_FileUtils(JSContext *cx, JSObject *global);
void register_all_cocos2dx(JSContext* cx, JSObject* obj);
bool js_cocos2dx_FileUtils_fullPathForFilename(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getStringFromFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_removeFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_isAbsolutePath(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_renameFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_loadFilenameLookupDictionaryFromFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_isPopupNotify(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getValueVectorFromFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getSearchPaths(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_writeToFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getValueMapFromFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getValueMapFromData(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_removeDirectory(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_setSearchPaths(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getFileSize(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_setSearchResolutionsOrder(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_addSearchResolutionsOrder(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_addSearchPath(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_isFileExist(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_purgeCachedEntries(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_fullPathFromRelativeFile(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_setPopupNotify(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_isDirectoryExist(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getSearchResolutionsOrder(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_createDirectory(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getWritablePath(JSContext *cx, uint32_t argc, jsval *vp);
bool js_cocos2dx_FileUtils_getInstance(JSContext *cx, uint32_t argc, jsval *vp);
