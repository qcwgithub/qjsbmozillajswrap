#pragma once

#include "jsapi.h"
#include "mozilla/Maybe.h"
#include <string>
#include "cocos/spidermonkey_specifics.h"
#include <thread>

class jsdebugger
{
private:
    JSRuntime *_rt;
	JSContext *_cx;
    mozilla::Maybe<JS::PersistentRootedObject> _global;
    mozilla::Maybe<JS::PersistentRootedObject> _debugGlobal;
	void cleanScript(const char *path);
	JSScript* getScript(const char *path);
	void enableDebugger(unsigned int port = 5086);
	void cleanup();
public:
	static JSClass   *_gclass;
	jsdebugger() : _rt(NULL), _cx(NULL)
    {
    }
	~jsdebugger();
    static jsdebugger* pInstance;
    static jsdebugger *getInstance() {
		if (pInstance == NULL) {
			pInstance = new jsdebugger();
		}
		return pInstance;
	};
    static void deleteInstance() {
        if (pInstance)
            delete pInstance;
        pInstance = NULL;
    }
	void debugProcessInput(const std::string& str);
	JSContext* getGlobalContext() {
		return _cx;
	};
	void compileScript(const char *path, JSObject* global, JSContext* cx);
	bool runScript(const char *path, JSObject* global = NULL, JSContext* cx = NULL);
	static void update(float dt);
	static void Clean();
	void Start(JSContext* cx, JSObject* global, JSClass* gclass, const char** src_searchpath, int nums, int port);
    JSObject* getDebugGlobal() { return _debugGlobal.ref().get(); }
    JSObject* getGlobalObject() { return _global.ref().get(); }
	static bool executeScript(JSContext *cx, uint32_t argc, jsval *vp);
	static bool forceGC(JSContext *cx, uint32_t argc, jsval *vp);
	static bool dumpRoot(JSContext *cx, uint32_t argc, jsval *vp);
	static bool addRootJS(JSContext *cx, uint32_t argc, jsval *vp);
	static bool removeRootJS(JSContext *cx, uint32_t argc, jsval *vp);
	static bool log(JSContext* cx, uint32_t argc, jsval *vp);
	//static void reset();
};

bool jsb_set_reserved_slot(JSObject *obj, uint32_t idx, jsval value);
bool jsb_get_reserved_slot(JSObject *obj, uint32_t idx, jsval& ret);

js_proxy_t* jsb_new_proxy(void* nativeObj, JSObject* jsObj);
js_proxy_t* jsb_get_native_proxy(void* nativeObj);
js_proxy_t* jsb_get_js_proxy(JSObject* jsObj);
void jsb_remove_proxy(js_proxy_t* nativeProxy, js_proxy_t* jsProxy);

