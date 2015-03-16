
#include "jsb_fileutils.h"
#include "../global_define.h"
#include "CCFileUtils.h"
#include "../jsdebugger.h"
#include "js_manual_conversions.h"
#include "spidermonkey_specifics.h"


JSClass  *jsb_cocos2d_FileUtils_class;
JSObject *jsb_cocos2d_FileUtils_prototype;

bool js_cocos2dx_FileUtils_fullPathForFilename(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_fullPathForFilename : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_fullPathForFilename : Error processing arguments");
		std::string ret = cobj->fullPathForFilename(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = std_string_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_fullPathForFilename : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_getStringFromFile(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getStringFromFile : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_getStringFromFile : Error processing arguments");
		std::string ret = cobj->getStringFromFile(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = std_string_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_getStringFromFile : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_removeFile(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_removeFile : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_removeFile : Error processing arguments");
		bool ret = cobj->removeFile(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_removeFile : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_isAbsolutePath(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_isAbsolutePath : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_isAbsolutePath : Error processing arguments");
		bool ret = cobj->isAbsolutePath(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_isAbsolutePath : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_renameFile(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_renameFile : Invalid Native Object");
	if (argc == 3) {
		std::string arg0;
		std::string arg1;
		std::string arg2;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		ok &= jsval_to_std_string(cx, argv[1], &arg1);
		ok &= jsval_to_std_string(cx, argv[2], &arg2);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_renameFile : Error processing arguments");
		bool ret = cobj->renameFile(arg0, arg1, arg2);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_renameFile : wrong number of arguments: %d, was expecting %d", argc, 3);
	return false;
}

/*
bool js_cocos2dx_FileUtils_loadFilenameLookupDictionaryFromFile(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_loadFilenameLookupDictionaryFromFile : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_loadFilenameLookupDictionaryFromFile : Error processing arguments");
		cobj->loadFilenameLookupDictionaryFromFile(arg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_loadFilenameLookupDictionaryFromFile : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
*/

bool js_cocos2dx_FileUtils_isPopupNotify(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_isPopupNotify : Invalid Native Object");
	if (argc == 0) {
		bool ret = cobj->isPopupNotify();
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_isPopupNotify : wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

/*
bool js_cocos2dx_FileUtils_getValueVectorFromFile(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getValueVectorFromFile : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_getValueVectorFromFile : Error processing arguments");
		ValueVector ret = cobj->getValueVectorFromFile(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = ccvaluevector_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_getValueVectorFromFile : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}*/

bool js_cocos2dx_FileUtils_getSearchPaths(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getSearchPaths : Invalid Native Object");
	if (argc == 0) {
		const std::vector<std::string>& ret = cobj->getSearchPaths();
		jsval jsret = JSVAL_NULL;
		jsret = std_vector_string_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_getSearchPaths : wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
//bool js_cocos2dx_FileUtils_writeToFile(JSContext *cx, uint32_t argc, jsval *vp)
//{
//	jsval *argv = JS_ARGV(cx, vp);
//	bool ok = true;
//	JSObject *obj = JS_THIS_OBJECT(cx, vp);
//	js_proxy_t *proxy = jsb_get_js_proxy(obj);
//	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
//	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_writeToFile : Invalid Native Object");
//	if (argc == 2) {
//		ValueMap arg0;
//		std::string arg1;
//		ok &= jsval_to_ccvaluemap(cx, argv[0], &arg0);
//		ok &= jsval_to_std_string(cx, argv[1], &arg1);
//		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_writeToFile : Error processing arguments");
//		bool ret = cobj->writeToFile(arg0, arg1);
//		jsval jsret = JSVAL_NULL;
//		jsret = BOOLEAN_TO_JSVAL(ret);
//		JS_SET_RVAL(cx, vp, jsret);
//		return true;
//	}
//
//	JS_ReportError(cx, "js_cocos2dx_FileUtils_writeToFile : wrong number of arguments: %d, was expecting %d", argc, 2);
//	return false;
//}
//bool js_cocos2dx_FileUtils_getValueMapFromFile(JSContext *cx, uint32_t argc, jsval *vp)
//{
//	jsval *argv = JS_ARGV(cx, vp);
//	bool ok = true;
//	JSObject *obj = JS_THIS_OBJECT(cx, vp);
//	js_proxy_t *proxy = jsb_get_js_proxy(obj);
//	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
//	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getValueMapFromFile : Invalid Native Object");
//	if (argc == 1) {
//		std::string arg0;
//		ok &= jsval_to_std_string(cx, argv[0], &arg0);
//		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_getValueMapFromFile : Error processing arguments");
//		ValueMap ret = cobj->getValueMapFromFile(arg0);
//		jsval jsret = JSVAL_NULL;
//		jsret = ccvaluemap_to_jsval(cx, ret);
//		JS_SET_RVAL(cx, vp, jsret);
//		return true;
//	}
//
//	JS_ReportError(cx, "js_cocos2dx_FileUtils_getValueMapFromFile : wrong number of arguments: %d, was expecting %d", argc, 1);
//	return false;
//}
//bool js_cocos2dx_FileUtils_getValueMapFromData(JSContext *cx, uint32_t argc, jsval *vp)
//{
//	jsval *argv = JS_ARGV(cx, vp);
//	bool ok = true;
//	JSObject *obj = JS_THIS_OBJECT(cx, vp);
//	js_proxy_t *proxy = jsb_get_js_proxy(obj);
//	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
//	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getValueMapFromData : Invalid Native Object");
//	if (argc == 2) {
//		const char* arg0;
//		int arg1;
//		std::string arg0_tmp; ok &= jsval_to_std_string(cx, argv[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
//		ok &= jsval_to_int32(cx, argv[1], (int32_t *)&arg1);
//		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_getValueMapFromData : Error processing arguments");
//		ValueMap ret = cobj->getValueMapFromData(arg0, arg1);
//		jsval jsret = JSVAL_NULL;
//		jsret = ccvaluemap_to_jsval(cx, ret);
//		JS_SET_RVAL(cx, vp, jsret);
//		return true;
//	}
//
//	JS_ReportError(cx, "js_cocos2dx_FileUtils_getValueMapFromData : wrong number of arguments: %d, was expecting %d", argc, 2);
//	return false;
//}
bool js_cocos2dx_FileUtils_removeDirectory(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_removeDirectory : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_removeDirectory : Error processing arguments");
		bool ret = cobj->removeDirectory(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_removeDirectory : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_setSearchPaths(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_setSearchPaths : Invalid Native Object");
	if (argc == 1) {
		std::vector<std::string> arg0;
		ok &= jsval_to_std_vector_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_setSearchPaths : Error processing arguments");
		cobj->setSearchPaths(arg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_setSearchPaths : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_getFileSize(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getFileSize : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_getFileSize : Error processing arguments");
		long ret = cobj->getFileSize(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = long_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_getFileSize : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_setSearchResolutionsOrder(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_setSearchResolutionsOrder : Invalid Native Object");
	if (argc == 1) {
		std::vector<std::string> arg0;
		ok &= jsval_to_std_vector_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_setSearchResolutionsOrder : Error processing arguments");
		cobj->setSearchResolutionsOrder(arg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_setSearchResolutionsOrder : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_addSearchResolutionsOrder(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_addSearchResolutionsOrder : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_addSearchResolutionsOrder : Error processing arguments");
		cobj->addSearchResolutionsOrder(arg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}
	if (argc == 2) {
		std::string arg0;
		bool arg1;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		arg1 = JS::ToBoolean(JS::RootedValue(cx, argv[1]));
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_addSearchResolutionsOrder : Error processing arguments");
		cobj->addSearchResolutionsOrder(arg0, arg1);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_addSearchResolutionsOrder : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_addSearchPath(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_addSearchPath : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_addSearchPath : Error processing arguments");
		cobj->addSearchPath(arg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}
	if (argc == 2) {
		std::string arg0;
		bool arg1;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		arg1 = JS::ToBoolean(JS::RootedValue(cx, argv[1]));
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_addSearchPath : Error processing arguments");
		cobj->addSearchPath(arg0, arg1);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_addSearchPath : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_isFileExist(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_isFileExist : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_isFileExist : Error processing arguments");
		bool ret = cobj->isFileExist(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_isFileExist : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_purgeCachedEntries(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_purgeCachedEntries : Invalid Native Object");
	if (argc == 0) {
		cobj->purgeCachedEntries();
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_purgeCachedEntries : wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_FileUtils_fullPathFromRelativeFile(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_fullPathFromRelativeFile : Invalid Native Object");
	if (argc == 2) {
		std::string arg0;
		std::string arg1;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		ok &= jsval_to_std_string(cx, argv[1], &arg1);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_fullPathFromRelativeFile : Error processing arguments");
		std::string ret = cobj->fullPathFromRelativeFile(arg0, arg1);
		jsval jsret = JSVAL_NULL;
		jsret = std_string_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_fullPathFromRelativeFile : wrong number of arguments: %d, was expecting %d", argc, 2);
	return false;
}
bool js_cocos2dx_FileUtils_setPopupNotify(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_setPopupNotify : Invalid Native Object");
	if (argc == 1) {
		bool arg0;
		arg0 = JS::ToBoolean(JS::RootedValue(cx, argv[0]));
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_setPopupNotify : Error processing arguments");
		cobj->setPopupNotify(arg0);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_setPopupNotify : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_isDirectoryExist(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_isDirectoryExist : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_isDirectoryExist : Error processing arguments");
		bool ret = cobj->isDirectoryExist(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_isDirectoryExist : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_getSearchResolutionsOrder(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getSearchResolutionsOrder : Invalid Native Object");
	if (argc == 0) {
		const std::vector<std::string>& ret = cobj->getSearchResolutionsOrder();
		jsval jsret = JSVAL_NULL;
		jsret = std_vector_string_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_getSearchResolutionsOrder : wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_FileUtils_createDirectory(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	bool ok = true;
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_createDirectory : Invalid Native Object");
	if (argc == 1) {
		std::string arg0;
		ok &= jsval_to_std_string(cx, argv[0], &arg0);
		JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_FileUtils_createDirectory : Error processing arguments");
		bool ret = cobj->createDirectory(arg0);
		jsval jsret = JSVAL_NULL;
		jsret = BOOLEAN_TO_JSVAL(ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_createDirectory : wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
bool js_cocos2dx_FileUtils_getWritablePath(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	FileUtils* cobj = (FileUtils *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "js_cocos2dx_FileUtils_getWritablePath : Invalid Native Object");
	if (argc == 0) {
		std::string ret = cobj->getWritablePath();
		jsval jsret = JSVAL_NULL;
		jsret = std_string_to_jsval(cx, ret);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}

	JS_ReportError(cx, "js_cocos2dx_FileUtils_getWritablePath : wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_FileUtils_getInstance(JSContext *cx, uint32_t argc, jsval *vp)
{
	if (argc == 0) {
		FileUtils* ret = FileUtils::getInstance();
		jsval jsret = JSVAL_NULL;
		do {
			if (ret) {
				js_proxy_t *jsProxy = js_get_or_create_proxy<FileUtils>(cx, (FileUtils*)ret);
				jsret = OBJECT_TO_JSVAL(jsProxy->obj);
			}
			else {
				jsret = JSVAL_NULL;
			}
		} while (0);
		JS_SET_RVAL(cx, vp, jsret);
		return true;
	}
	JS_ReportError(cx, "js_cocos2dx_FileUtils_getInstance : wrong number of arguments");
	return false;
}



void js_cocos2d_FileUtils_finalize(JSFreeOp *fop, JSObject *obj) {
	CCLOGINFO("jsbindings: finalizing JS object %p (FileUtils)", obj);
}

static bool js_is_native_obj(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	vp.set(BOOLEAN_TO_JSVAL(true));
	return true;
}

static bool empty_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
	return false;
}


void js_register_cocos2dx_FileUtils(JSContext *cx, JSObject *global) {
	jsb_cocos2d_FileUtils_class = (JSClass *)calloc(1, sizeof(JSClass));
	jsb_cocos2d_FileUtils_class->name = "FileUtils";
	jsb_cocos2d_FileUtils_class->addProperty = JS_PropertyStub;
	jsb_cocos2d_FileUtils_class->delProperty = JS_DeletePropertyStub;
	jsb_cocos2d_FileUtils_class->getProperty = JS_PropertyStub;
	jsb_cocos2d_FileUtils_class->setProperty = JS_StrictPropertyStub;
	jsb_cocos2d_FileUtils_class->enumerate = JS_EnumerateStub;
	jsb_cocos2d_FileUtils_class->resolve = JS_ResolveStub;
	jsb_cocos2d_FileUtils_class->convert = JS_ConvertStub;
	jsb_cocos2d_FileUtils_class->finalize = js_cocos2d_FileUtils_finalize;
	jsb_cocos2d_FileUtils_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

	static JSPropertySpec properties[] = {
			{ "__nativeObj", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, JSOP_WRAPPER(js_is_native_obj), JSOP_NULLWRAPPER },
			{ 0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER }
	};

	static JSFunctionSpec funcs[] = {
		JS_FN("fullPathForFilename", js_cocos2dx_FileUtils_fullPathForFilename, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("getStringFromFile", js_cocos2dx_FileUtils_getStringFromFile, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("removeFile", js_cocos2dx_FileUtils_removeFile, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("isAbsolutePath", js_cocos2dx_FileUtils_isAbsolutePath, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("renameFile", js_cocos2dx_FileUtils_renameFile, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//JS_FN("loadFilenameLookup", js_cocos2dx_FileUtils_loadFilenameLookupDictionaryFromFile, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("isPopupNotify", js_cocos2dx_FileUtils_isPopupNotify, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//JS_FN("getValueVectorFromFile", js_cocos2dx_FileUtils_getValueVectorFromFile, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("getSearchPaths", js_cocos2dx_FileUtils_getSearchPaths, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//JS_FN("writeToFile", js_cocos2dx_FileUtils_writeToFile, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//JS_FN("getValueMapFromFile", js_cocos2dx_FileUtils_getValueMapFromFile, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//JS_FN("getValueMapFromData", js_cocos2dx_FileUtils_getValueMapFromData, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("removeDirectory", js_cocos2dx_FileUtils_removeDirectory, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("setSearchPaths", js_cocos2dx_FileUtils_setSearchPaths, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("getFileSize", js_cocos2dx_FileUtils_getFileSize, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("setSearchResolutionsOrder", js_cocos2dx_FileUtils_setSearchResolutionsOrder, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("addSearchResolutionsOrder", js_cocos2dx_FileUtils_addSearchResolutionsOrder, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("addSearchPath", js_cocos2dx_FileUtils_addSearchPath, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("isFileExist", js_cocos2dx_FileUtils_isFileExist, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("purgeCachedEntries", js_cocos2dx_FileUtils_purgeCachedEntries, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("fullPathFromRelativeFile", js_cocos2dx_FileUtils_fullPathFromRelativeFile, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("setPopupNotify", js_cocos2dx_FileUtils_setPopupNotify, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("isDirectoryExist", js_cocos2dx_FileUtils_isDirectoryExist, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("getSearchResolutionsOrder", js_cocos2dx_FileUtils_getSearchResolutionsOrder, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("createDirectory", js_cocos2dx_FileUtils_createDirectory, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("getWritablePath", js_cocos2dx_FileUtils_getWritablePath, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FS_END
	};

	static JSFunctionSpec st_funcs[] = {
		JS_FN("getInstance", js_cocos2dx_FileUtils_getInstance, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FS_END
	};

	jsb_cocos2d_FileUtils_prototype = JS_InitClass(
		cx, global,
		NULL, // parent proto
		jsb_cocos2d_FileUtils_class,
		empty_constructor, 0,
		properties,
		funcs,
		NULL, // no static properties
		st_funcs);
	// make the class enumerable in the registered namespace
	//  bool found;
	//FIXME: Removed in Firefox v27 
	//  JS_SetPropertyAttributes(cx, global, "FileUtils", JSPROP_ENUMERATE | JSPROP_READONLY, &found);

	// add the proto and JSClass to the type->js info hash table
	TypeTest<FileUtils> t;
	js_type_class_t *p;
	std::string typeName = t.s_name();
	if (_js_global_type_map.find(typeName) == _js_global_type_map.end())
	{
		p = (js_type_class_t *)malloc(sizeof(js_type_class_t));
		p->jsclass = jsb_cocos2d_FileUtils_class;
		p->proto = jsb_cocos2d_FileUtils_prototype;
		p->parentProto = NULL;
		_js_global_type_map.insert(std::make_pair(typeName, p));
	}
}