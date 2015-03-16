#pragma once
#include "jsapi.h"
#include "jsfriendapi.h"
#include <string>
#include <vector>
#include "spidermonkey_specifics.h"
#include "../global_define.h"

class JSStringWrapper
{
public:
	JSStringWrapper();
	JSStringWrapper(JSString* str, JSContext* cx = NULL);
	JSStringWrapper(jsval val, JSContext* cx = NULL);
	~JSStringWrapper();

	void set(jsval val, JSContext* cx);
	void set(JSString* str, JSContext* cx);
	const char* get();

private:
	const char* _buffer;

private:
	//JSStringWrapper(const JSStringWrapper &) = delete;
	//JSStringWrapper &operator =(const JSStringWrapper &) = delete;
};

bool jsval_to_std_string(JSContext *cx, jsval v, std::string* ret);
bool jsval_to_std_wstring(JSContext* cx, jsval v, std::wstring* ret);
jsval std_string_to_jsval(JSContext* cx, const std::string& v);
jsval c_string_to_jsval(JSContext* cx, const char* v, size_t length /* = 0 */);
jsval std_vector_string_to_jsval(JSContext *cx, const std::vector<std::string>& v);
bool jsval_to_std_vector_string(JSContext *cx, jsval vp, std::vector<std::string>* ret);
jsval long_to_jsval(JSContext *cx, long number);
jsval wchar_to_jsval(JSContext* cx, unsigned short* v, size_t length = 0);
jsval std_wstring_to_jsval(JSContext* cx, const std::wstring& v);
//int wcslen(const unsigned short* str);
unsigned short* utf8_to_utf16(const char* str_old, int length/* = -1*/, int* rUtf16Size/* = nullptr*/);
char * utf16_to_utf8(const unsigned short *str, int len, long *items_read, long *items_written);

/**
* You don't need to manage the returned pointer. They live for the whole life of
* the app.
*/
template <class T>
inline js_type_class_t *js_get_type_from_native(T* native_obj) {
	bool found = false;
	std::string typeName = typeid(*native_obj).name();
	auto typeProxyIter = _js_global_type_map.find(typeName);
	if (typeProxyIter == _js_global_type_map.end())
	{
		typeName = typeid(T).name();
		typeProxyIter = _js_global_type_map.find(typeName);
		if (typeProxyIter != _js_global_type_map.end())
		{
			found = true;
		}
	}
	else
	{
		found = true;
	}
	return found ? typeProxyIter->second : nullptr;
}

/**
* The returned pointer should be deleted using jsb_remove_proxy. Most of the
* time you do that in the C++ destructor.
*/
template<class T>
inline js_proxy_t *js_get_or_create_proxy(JSContext *cx, T *native_obj) {
	js_proxy_t *proxy;
	HASH_FIND_PTR(_native_js_global_ht, &native_obj, proxy);
	if (!proxy) {
		js_type_class_t *typeProxy = js_get_type_from_native<T>(native_obj);
		// Return NULL if can't find its type rather than making an assert.
		//        assert(typeProxy);
		if (!typeProxy) {
			CCLOGINFO("Could not find the type of native object.");
			return NULL;
		}

		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

			JSObject* js_obj = JS_NewObject(cx, typeProxy->jsclass, typeProxy->proto, typeProxy->parentProto);
		proxy = jsb_new_proxy(native_obj, js_obj);
#ifdef DEBUG
		JS_AddNamedObjectRoot(cx, &proxy->obj, typeid(*native_obj).name());
#else
		JS_AddObjectRoot(cx, &proxy->obj);
#endif
		return proxy;
	}
	else {
		return proxy;
	}
	return NULL;
}
