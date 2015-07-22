
#pragma warning( disable : 4996)

#if (TARGET_PLATFORM == PLATFORM_WIN32)
#pragma comment(lib, "Ws2_32.lib")
#endif

#include "jsdebugger.h"
#include <iostream>
#include <thread>
#include "cocos/jsb_fileutils.h"


// Removed in Firefox v27, use 'js/OldDebugAPI.h' instead
//#include "jsdbgapi.h"
#include "js/OldDebugAPI.h"
#include "cocos/js_manual_conversions.h"
#include "cocos/CCData.h"
#include "cocos/CCFileUtils.h"
#include "global_define.h"


// for debug socket
#if (TARGET_PLATFORM == PLATFORM_WIN32)
#include <io.h>
#include <WS2tcpip.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

#else
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <thread>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <sstream>
#include <unordered_map>
#include <mutex>

#ifdef ANDROID
#include <android/log.h>
#include "cocos/android/JniHelper.h"
#include <netinet/in.h>
#endif

#define BYTE_CODE_FILE_EXT ".jsc"

#ifdef ANDROID
#define  LOG_TAG    "jsdebugger.cpp"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...) js_log(__VA_ARGS__)
#endif



static std::string inData;
static std::string outData;
static std::vector<std::string> g_queue;
static std::mutex g_qMutex;
static std::mutex g_rwMutex;
static int clientSocket = -1;
static uint32_t s_nestedLoopLevel = 0;

js_proxy_t *_native_js_global_ht = NULL;
js_proxy_t *_js_native_global_ht = NULL;
std::unordered_map<std::string, js_type_class_t*> _js_global_type_map;

static char *_js_log_buf = NULL;
/// The max length of CCLog message.
static const int MAX_LOG_LENGTH = 16 * 1024;
// name ~> JSScript map
static std::unordered_map<std::string, JSScript*> filename_script;
// port ~> socket map
//static std::unordered_map<int, int> ports_sockets;
// name ~> globals
//static std::unordered_map<std::string, JS::RootedObject*> globals;

static void cc_closesocket(int fd)
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	closesocket(fd);
#else
	close(fd);
#endif
}

void js_log(const char *format, ...) {

	if (_js_log_buf == NULL)
	{
		_js_log_buf = (char *)calloc(sizeof(char), MAX_LOG_LENGTH + 1);
		_js_log_buf[MAX_LOG_LENGTH] = '\0';
	}
	va_list vl;
	va_start(vl, format);
	int len = vsnprintf(_js_log_buf, MAX_LOG_LENGTH, format, vl);
	va_end(vl);
	if (len > 0)
	{
		std::cout << "JS: " << _js_log_buf << std::endl;
		//CCLOG("JS: %s", _js_log_buf);
	}
}

static void _clientSocketWriteAndClearString(std::string& s)
{
	::send(clientSocket, s.c_str(), s.length(), 0);
	s.clear();
}

static void processInput(const std::string& data) {
	std::lock_guard<std::mutex> lk(g_qMutex);
	g_queue.push_back(data);
}

static void clearBuffers() {
	std::lock_guard<std::mutex> lk(g_rwMutex);
	// only process input if there's something and we're not locked
	if (inData.length() > 0) {
		processInput(inData);
		inData.clear();
	}
	if (outData.length() > 0) {
		_clientSocketWriteAndClearString(outData);
	}
}


jsdebugger::~jsdebugger()
{
	cleanup();
}

//extern JSClass global_class;
JSClass* jsdebugger::_gclass = NULL;
int server_socket = 0;
std::vector<int> client_sockets;

JSObject* NewGlobalObject(JSContext* cx, bool debug)
{
	JS::CompartmentOptions options;
	options.setVersion(JSVERSION_LATEST);

	JS::RootedObject glob(cx, JS_NewGlobalObject(cx, jsdebugger::_gclass, NULL, JS::DontFireOnNewGlobalHook, options));
	if (!glob) {
		return NULL;
	}
	JSAutoCompartment ac(cx, glob);
	bool ok = true;
	ok = JS_InitStandardClasses(cx, glob);
	if (ok)
		JS_InitReflect(cx, glob);
	if (ok && debug)
		ok = JS_DefineDebuggerObject(cx, glob);
	if (!ok)
		return NULL;

	JS_FireOnNewGlobalObject(cx, glob);

	return glob;
}

static void serverEntryPoint(unsigned int port)
{
	// start a server, accept the connection and keep reading data from it
	struct addrinfo hints, *result = nullptr, *rp = nullptr;
	server_socket = 0;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;       // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	std::stringstream portstr;
	portstr << port;

	int err = 0;

#if (TARGET_PLATFORM == PLATFORM_WIN32)
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	if ((err = getaddrinfo(NULL, portstr.str().c_str(), &hints, &result)) != 0) {
		LOGD("getaddrinfo error : %s\n", gai_strerror(err));
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if ((server_socket = socket(rp->ai_family, rp->ai_socktype, 0)) < 0) {
			continue;
		}
		int optval = 1;
		if ((setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval))) < 0) {
			cc_closesocket(server_socket);
			TRACE_DEBUGGER_SERVER("debug server : error setting socket option SO_REUSEADDR");
			return;
		}

#if (TARGET_PLATFORM == PLATFORM_IOS)
		if ((setsockopt(server_socket, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval))) < 0) {
			close(server_socket);
			TRACE_DEBUGGER_SERVER("debug server : error setting socket option SO_NOSIGPIPE");
			return;
		}
#endif //(TARGET_PLATFORM == PLATFORM_IOS)

		if ((::bind(server_socket, rp->ai_addr, rp->ai_addrlen)) == 0) {
			break;
		}
		cc_closesocket(server_socket);
		server_socket = -1;
	}
	if (server_socket < 0 || rp == NULL) {
		TRACE_DEBUGGER_SERVER("debug server : error creating/binding socket");
		return;
	}

	freeaddrinfo(result);

	listen(server_socket, 1);

	while (true) {
		clientSocket = accept(server_socket, NULL, NULL);

		if (clientSocket < 0)
		{
			TRACE_DEBUGGER_SERVER("debug server : error on accept");
			return;
		}
		else
		{
			client_sockets.push_back(clientSocket);
			// read/write data
			TRACE_DEBUGGER_SERVER("debug server : client connected");

			inData = "connected";
			// process any input, send any output
			clearBuffers();

			char buf[1024] = { 0 };
			int readBytes = 0;
			while ((readBytes = (int)::recv(clientSocket, buf, sizeof(buf), 0)) > 0)
			{
				buf[readBytes] = '\0';
				// TRACE_DEBUGGER_SERVER("debug server : received command >%s", buf);

				// no other thread is using this
				inData.append(buf);
				// process any input, send any output
				clearBuffers();
			} // while(read)

			cc_closesocket(clientSocket);
		}
	} // while(true)
}

static bool NS_ProcessNextEvent()
{
	g_qMutex.lock();
	size_t size = g_queue.size();
	g_qMutex.unlock();

	while (size > 0)
	{
		g_qMutex.lock();
		auto first = g_queue.begin();
		std::string str = *first;
		g_queue.erase(first);
		size = g_queue.size();
		g_qMutex.unlock();

		jsdebugger::getInstance()->debugProcessInput(str);
	}
	//    std::this_thread::yield();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

void jsdebugger::debugProcessInput(const std::string& str)
{
	JSAutoCompartment ac(_cx, _debugGlobal.ref());
    
    JSString* jsstr = JS_NewStringCopyZ(_cx, str.c_str());
    jsval argv = STRING_TO_JSVAL(jsstr);
    JS::RootedValue outval(_cx);
    
    JS_CallFunctionName(_cx, JS::RootedObject(_cx, _debugGlobal.ref()), "processInput", JS::HandleValueArray::fromMarkedLocation(1, &argv), &outval);
}

bool JSBDebug_BufferWrite(JSContext* cx, unsigned argc, jsval* vp)
{
	if (argc == 1) {
		jsval* argv = JS_ARGV(cx, vp);
		JSStringWrapper strWrapper(argv[0]);
		// this is safe because we're already inside a lock (from clearBuffers)
		outData.append(strWrapper.get());
		_clientSocketWriteAndClearString(outData);
	}
	return true;
}

bool JSBDebug_enterNestedEventLoop(JSContext* cx, unsigned argc, jsval* vp)
{
    enum {
        NS_OK = 0,
        NS_ERROR_UNEXPECTED
    };
    
#define NS_SUCCEEDED(v) ((v) == NS_OK)
    
    int rv = NS_OK;
    
    uint32_t nestLevel = ++s_nestedLoopLevel;

    while (NS_SUCCEEDED(rv) && s_nestedLoopLevel >= nestLevel) {
        if (!NS_ProcessNextEvent())
            rv = NS_ERROR_UNEXPECTED;
    }
    
    CCASSERT(s_nestedLoopLevel <= nestLevel,
             "nested event didn't unwind properly");
    
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    args.rval().set(UINT_TO_JSVAL(s_nestedLoopLevel));
//    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(s_nestedLoopLevel));
    return true;
}

bool JSBDebug_exitNestedEventLoop(JSContext* cx, unsigned argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    if (s_nestedLoopLevel > 0) {
        --s_nestedLoopLevel;
    } else {
        args.rval().set(UINT_TO_JSVAL(0));
//        JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(0));
        return true;
    }
    args.rval().setUndefined();
//    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(s_nestedLoopLevel));
    return true;
}

bool JSBDebug_getEventLoopNestLevel(JSContext* cx, unsigned argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    args.rval().set(UINT_TO_JSVAL(s_nestedLoopLevel));
//    JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(s_nestedLoopLevel));
    return true;
}

void jsdebugger::enableDebugger(unsigned int port /*= 5086*/)
{
	if (_debugGlobal.empty())
	{
        JSAutoCompartment ac0(_cx, _global.ref().get());

		JS_SetDebugMode(_cx, true);

        _debugGlobal.construct(_cx);
		_debugGlobal.ref() = NewGlobalObject(_cx, true);
		// Adds the debugger object to root, otherwise it may be collected by GC.
        //AddObjectRoot(_cx, &_debugGlobal.ref()); no need, it's persistent rooted now
        //JS_WrapObject(_cx, &_debugGlobal.ref()); Not really needed, JS_WrapObject makes a cross-compartment wrapper for the given JS object
        JS::RootedObject rootedDebugObj(_cx, _debugGlobal.ref().get());
		
		JSAutoCompartment ac(_cx, rootedDebugObj);
		// these are used in the debug program
		JS_DefineFunction(_cx, rootedDebugObj, "log", jsdebugger::log, 0, JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT);
		JS_DefineFunction(_cx, rootedDebugObj, "_bufferWrite", JSBDebug_BufferWrite, 1, JSPROP_READONLY | JSPROP_PERMANENT);
		JS_DefineFunction(_cx, rootedDebugObj, "_enterNestedEventLoop", JSBDebug_enterNestedEventLoop, 0, JSPROP_READONLY | JSPROP_PERMANENT);
		JS_DefineFunction(_cx, rootedDebugObj, "_exitNestedEventLoop", JSBDebug_exitNestedEventLoop, 0, JSPROP_READONLY | JSPROP_PERMANENT);
		JS_DefineFunction(_cx, rootedDebugObj, "_getEventLoopNestLevel", JSBDebug_getEventLoopNestLevel, 0, JSPROP_READONLY | JSPROP_PERMANENT);

		runScript("debug/jsb_debugger.javascript", rootedDebugObj);

        JS::RootedObject globalObj(_cx, _global.ref().get());
        JS_WrapObject(_cx, &globalObj);
		// prepare the debugger
		jsval argv = OBJECT_TO_JSVAL(globalObj);
		JS::RootedValue outval(_cx);
		bool ok = JS_CallFunctionName(_cx, rootedDebugObj, "_prepareDebugger", JS::HandleValueArray::fromMarkedLocation(1, &argv), &outval);
		if (!ok) {
			JS_ReportPendingException(_cx);
		}

		// start bg thread
		auto t = std::thread(&serverEntryPoint, port);
		t.detach();

		/*
		Scheduler* scheduler = Director::getInstance()->getScheduler();
		scheduler->scheduleUpdate(this->_runLoop, 0, false);
		*/
	}
}







bool jsdebugger::log(JSContext* cx, uint32_t argc, jsval *vp)
{
	if (argc > 0) {
		JSString *string = NULL;
		JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &string);
		if (string) {
			JSStringWrapper wrapper(string);
			js_log("%s", wrapper.get());
		}
	}
	return true;
}


bool jsdebugger::runScript(const char *path, JSObject* global, JSContext* cx)
{
	if (global == NULL) {
		global = _global;
	}
	if (cx == NULL) {
		cx = _cx;
	}
	compileScript(path, global, cx);
	JSScript * script = getScript(path);
	bool evaluatedOK = false;
	if (script) {
		jsval rval;
		JSAutoCompartment ac(cx, global);
		evaluatedOK = JS_ExecuteScript(cx, global, script, &rval);
		if (false == evaluatedOK) {
			//cocos2d::log("(evaluatedOK == JS_FALSE)");
			JS_ReportPendingException(cx);
		}
	}
	return evaluatedOK;
}

static std::string RemoveFileExt(const std::string& filePath) {
	size_t pos = filePath.rfind('.');
	if (0 < pos) {
		return filePath.substr(0, pos);
	}
	else {
		return filePath;
	}
}

JSScript* jsdebugger::getScript(const char *path)
{
	// a) check jsc file first
	std::string byteCodePath = RemoveFileExt(std::string(path)) + BYTE_CODE_FILE_EXT;
	if (filename_script[byteCodePath])
		return filename_script[byteCodePath];

	// b) no jsc file, check js file
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);
	if (filename_script[fullPath])
		return filename_script[fullPath];

	return NULL;
}

static void ReportException(JSContext *cx)
{
	if (JS_IsExceptionPending(cx)) {
		if (!JS_ReportPendingException(cx)) {
			JS_ClearPendingException(cx);
		}
	}
}

void jsdebugger::compileScript(const char *path, JSObject* global, JSContext* cx)
{
	if (!path) {
		return;
	}

	if (getScript(path)) {
		return;
	}

	FileUtils *futil = FileUtils::getInstance();

	if (global == NULL) {
		global = _global;
	}
	if (cx == NULL) {
		cx = _cx;
	}

	JSAutoCompartment ac(cx, global);

	JS::RootedScript script(cx);
	JS::RootedObject obj(cx, global);

	// a) check jsc file first
	std::string byteCodePath = RemoveFileExt(std::string(path)) + BYTE_CODE_FILE_EXT;

	// Check whether '.jsc' files exist to avoid outputing log which says 'couldn't find .jsc file'.
	if (futil->isFileExist(byteCodePath))
	{
		Data data = futil->getDataFromFile(byteCodePath);
		if (!data.isNull())
		{
			script = JS_DecodeScript(cx, data.getBytes(), static_cast<uint32_t>(data.getSize()), nullptr, nullptr);
		}
	}

	// b) no jsc file, check js file
	if (!script)
	{
		/* Clear any pending exception from previous failed decoding.  */
		ReportException(cx);

		std::string fullPath = futil->fullPathForFilename(path);
		JS::CompileOptions options(cx);
		options.setUTF8(true).setFileAndLine(fullPath.c_str(), 1);

#if (TARGET_PLATFORM == PLATFORM_ANDROID)
		std::string jsFileContent = futil->getStringFromFile(fullPath);
		if (!jsFileContent.empty())
		{
			script = JS::Compile(cx, obj, options, jsFileContent.c_str(), jsFileContent.size());
		}
#else
		script = JS::Compile(cx, obj, options, fullPath.c_str());
#endif
		if (script) {
			filename_script[fullPath] = script;
		}
	}
	else {
		filename_script[byteCodePath] = script;
	}
}

void jsdebugger::cleanScript(const char *path)
{
	std::string byteCodePath = RemoveFileExt(std::string(path)) + BYTE_CODE_FILE_EXT;
	auto it = filename_script.find(byteCodePath);
	if (it != filename_script.end())
	{
		filename_script.erase(it);
	}

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);
	it = filename_script.find(fullPath);
	if (it != filename_script.end())
	{
		filename_script.erase(it);
	}
}

void jsdebugger::update(float dt)
{
	g_qMutex.lock();
	size_t size = g_queue.size();
	g_qMutex.unlock();

	while (size > 0)
	{
		g_qMutex.lock();
		auto first = g_queue.begin();
		std::string str = *first;
		g_queue.erase(first);
		size = g_queue.size();
		g_qMutex.unlock();

		jsdebugger::getInstance()->debugProcessInput(str);
	}
}

bool jsdebugger::executeScript(JSContext *cx, uint32_t argc, jsval *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if (argc >= 1) {
        JSString* str = JS::ToString(cx, JS::RootedValue(cx, args.get(0)));
        JSStringWrapper path(str);

        printf("!REQUIRE %s\n", path.get());

        bool res = false;
        if (argc == 2 && args.get(1).isString()) {
            JSString* globalName = args.get(1).toString();
            JSStringWrapper name(globalName);

            JS::RootedObject debugObj(cx, jsdebugger::getInstance()->getDebugGlobal());
			if (debugObj) {
				res = jsdebugger::getInstance()->runScript(path.get(), debugObj);
			}
			else {
				JS_ReportError(cx, "Invalid global object: %s", name.get());
				return false;
			}
		}
		else {
            JS::RootedObject glob(cx, JS::CurrentGlobalOrNull(cx));
			res = jsdebugger::getInstance()->runScript(path.get(), glob);
		}
		return res;
	}
    args.rval().setUndefined();
    return true;
}

bool jsdebugger::forceGC(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSRuntime *rt = JS_GetRuntime(cx);
	JS_GC(rt);
	return true;
}

bool jsdebugger::dumpRoot(JSContext *cx, uint32_t argc, jsval *vp)
{
	// JS_DumpNamedRoots is only available on DEBUG versions of SpiderMonkey.
	// Mac and Simulator versions were compiled with DEBUG.
#if COCOS2D_DEBUG
	//    JSContext *_cx = ScriptingCore::getInstance()->getGlobalContext();
	//    JSRuntime *rt = JS_GetRuntime(_cx);
	//    JS_DumpNamedRoots(rt, dumpNamedRoot, NULL);
	//    JS_DumpHeap(rt, stdout, NULL, JSTRACE_OBJECT, NULL, 2, NULL);
#endif
	return true;
}

bool jsdebugger::addRootJS(JSContext *cx, uint32_t argc, jsval *vp)
{
	if (argc == 1) {
		JSObject *o = NULL;
		if (JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &o) == true) {
			if (JS_AddNamedObjectRoot(cx, &o, "from-js") == false) {
				LOGD("something went wrong when setting an object to the root");
			}
		}
		return true;
	}
	return false;
}

bool jsdebugger::removeRootJS(JSContext *cx, uint32_t argc, jsval *vp)
{
	if (argc == 1) {
		JSObject *o = NULL;
		if (JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "o", &o) == true) {
			JS_RemoveObjectRoot(cx, &o);
		}
		return true;
	}
	return false;
}

//void jsdebugger::reset()
//{
//	cleanup();
//	Start();
//}


bool JSBCore_platform(JSContext *cx, uint32_t argc, jsval *vp)
{
	if (argc != 0)
	{
		JS_ReportError(cx, "Invalid number of arguments in __getPlatform");
		return false;
	}

	//Application::Platform platform;

	// config.deviceType: Device Type
	// 'mobile' for any kind of mobile devices, 'desktop' for PCs, 'browser' for Web Browsers
	// #if (TARGET_PLATFORM == PLATFORM_WIN32) || (TARGET_PLATFORM == PLATFORM_LINUX) || (TARGET_PLATFORM == PLATFORM_MAC)
	//     platform = JS_InternString(_cx, "desktop");
	// #else
	//platform = Application::getInstance()->getTargetPlatform();
	// #endif

	jsval ret = INT_TO_JSVAL((int)1);

	JS_SET_RVAL(cx, vp, ret);

	return true;
};

bool JSBCore_version(JSContext *cx, uint32_t argc, jsval *vp)
{
	if (argc != 0)
	{
		JS_ReportError(cx, "Invalid number of arguments in __getVersion");
		return false;
	}

	char version[256];
	snprintf(version, sizeof(version) - 1, "%s", "unity-jsb-1.0");
	JSString * js_version = JS_InternString(cx, version);

	jsval ret = STRING_TO_JSVAL(js_version);
	JS_SET_RVAL(cx, vp, ret);

	return true;
};

bool JSBCore_os(JSContext *cx, uint32_t argc, jsval *vp)
{
	if (argc != 0)
	{
		JS_ReportError(cx, "Invalid number of arguments in __getOS");
		return false;
	}

	JSString * os;

	// osx, ios, android, windows, linux, etc..
#if (TARGET_PLATFORM == PLATFORM_IOS)
	os = JS_InternString(cx, "iOS");
#elif (TARGET_PLATFORM == PLATFORM_ANDROID)
	os = JS_InternString(cx, "Android");
#elif (TARGET_PLATFORM == PLATFORM_WIN32)
	os = JS_InternString(cx, "Windows");
#elif (TARGET_PLATFORM == PLATFORM_MARMALADE)
	os = JS_InternString(cx, "Marmalade");
#elif (TARGET_PLATFORM == PLATFORM_LINUX)
	os = JS_InternString(cx, "Linux");
#elif (TARGET_PLATFORM == PLATFORM_BADA)
	os = JS_InternString(cx, "Bada");
#elif (TARGET_PLATFORM == PLATFORM_BLACKBERRY)
	os = JS_InternString(cx, "Blackberry");
#elif (TARGET_PLATFORM == PLATFORM_MAC)
	os = JS_InternString(cx, "OS X");
#else
	os = JS_InternString(cx, "Unknown");
#endif

	jsval ret = STRING_TO_JSVAL(os);
	JS_SET_RVAL(cx, vp, ret);

	return true;
};

bool JSB_core_restartVM(JSContext *cx, uint32_t argc, jsval *vp)
{
	/*
	JSB_PRECONDITION2(argc == 0, cx, false, "Invalid number of arguments in executeScript");
	jsdebugger::getInstance()->reset();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	*/
	return false;
};

void registerDefaultClasses(JSContext* cx,JS::HandleObject global) {
	// first, try to get the ns
    JS::RootedValue nsval(cx);
    JS::RootedObject ns(cx);
    JS_GetProperty(cx, global, "cc", &nsval);
    // Not exist, create it
    if (nsval == JSVAL_VOID)
    {
        ns.set(JS_NewObject(cx, NULL, JS::NullPtr(), JS::NullPtr()));
        nsval = OBJECT_TO_JSVAL(ns);
        JS_SetProperty(cx, global, "cc", nsval);
    }
    else
    {
        ns.set(nsval.toObjectOrNull());
    }

    //
    // Javascript controller (__jsc__)
    //
    JS::RootedObject proto(cx);
    JS::RootedObject parent(cx);
    JS::RootedObject jsc(cx, JS_NewObject(cx, NULL, proto, parent));
    JS::RootedValue jscVal(cx);
    jscVal = OBJECT_TO_JSVAL(jsc);
    JS_SetProperty(cx, global, "__jsc__", jscVal);

	JS_DefineFunction(cx, jsc, "garbageCollect", jsdebugger::forceGC, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);
	JS_DefineFunction(cx, jsc, "dumpRoot", jsdebugger::dumpRoot, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);
	JS_DefineFunction(cx, jsc, "addGCRootObject", jsdebugger::addRootJS, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);
	JS_DefineFunction(cx, jsc, "removeGCRootObject", jsdebugger::removeRootJS, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);
	JS_DefineFunction(cx, jsc, "executeScript", jsdebugger::executeScript, 1, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);

	// register some global functions
	JS_DefineFunction(cx, global, "require", jsdebugger::executeScript, 1, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunction(cx, global, "log", jsdebugger::log, 0, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunction(cx, global, "executeScript", jsdebugger::executeScript, 1, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunction(cx, global, "forceGC", jsdebugger::forceGC, 0, JSPROP_READONLY | JSPROP_PERMANENT);

	JS_DefineFunction(cx, global, "__getPlatform", JSBCore_platform, 0, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunction(cx, global, "__getOS", JSBCore_os, 0, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunction(cx, global, "__getVersion", JSBCore_version, 0, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunction(cx, global, "__restartVM", JSB_core_restartVM, 0, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "__cleanScript", JSB_cleanScript, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, global, "__isObjectValid", ScriptingCore::isObjectValid, 1, JSPROP_READONLY | JSPROP_PERMANENT);
}

void registerCocos2dClasses(JSContext* cx, JS::HandleObject obj, const std::string &name, JS::MutableHandleObject jsObj)
{
    JS::RootedValue nsval(cx);
    JS_GetProperty(cx, obj, name.c_str(), &nsval);
    if (nsval == JSVAL_VOID) {
        jsObj.set(JS_NewObject(cx, NULL, JS::NullPtr(), JS::NullPtr()));
        nsval = OBJECT_TO_JSVAL(jsObj);
        JS_SetProperty(cx, obj, name.c_str(), nsval);
    } else {
        jsObj.set(nsval.toObjectOrNull());
    }
}

jsdebugger* jsdebugger::pInstance = NULL;

void jsdebugger::Start(JSContext* cx, JS::HandleObject global, JSClass* gclass, const char** src_searchpath, int nums, int port)
{
	std::vector<std::string> paths;
	for (int i = 0; i < nums; i++)
	{
		paths.push_back(src_searchpath[i]);
	}
	FileUtils::getInstance()->setSearchPaths(paths);
	_cx = cx;
	_global = global;
	_gclass = gclass;
	registerDefaultClasses(cx, global);

    JS::RootedObject jsbObj(cx);
    registerCocos2dClasses(cx, global, "jsb", &jsbObj);
	js_register_cocos2dx_FileUtils(cx, global);
	enableDebugger(port);
}

void jsdebugger::cleanup()
{
	js_proxy_t *current, *tmp;
	HASH_ITER(hh, _js_native_global_ht, current, tmp) {
		//JS_RemoveObjectRoot(cx, &current->obj);
		HASH_DEL(_js_native_global_ht, current);
		free(current);
	}
	HASH_ITER(hh, _native_js_global_ht, current, tmp) {
		HASH_DEL(_native_js_global_ht, current);
		free(current);
	}
	HASH_CLEAR(hh, _js_native_global_ht);
	HASH_CLEAR(hh, _native_js_global_ht);

	if (_js_log_buf) {
		free(_js_log_buf);
		_js_log_buf = NULL;
	}

	for (auto iter = _js_global_type_map.begin(); iter != _js_global_type_map.end(); ++iter)
	{
		free(iter->second->jsclass);
		free(iter->second);
	}

	_js_global_type_map.clear();
	filename_script.clear();

	if (server_socket > 0)
	{
		for (int i = 0; i < client_sockets.size(); i++){
			cc_closesocket(client_sockets[i]);
		}
		client_sockets.clear();
		cc_closesocket(server_socket);
		server_socket = 0;
	}
}

void jsdebugger::Clean()
{
	deleteInstance();
}


bool jsb_set_reserved_slot(JSObject *obj, uint32_t idx, jsval value)
{
	const JSClass *klass = JS_GetClass(obj);
	unsigned int slots = JSCLASS_RESERVED_SLOTS(klass);
	if (idx >= slots)
		return false;

	JS_SetReservedSlot(obj, idx, value);

	return true;
}

bool jsb_get_reserved_slot(JSObject *obj, uint32_t idx, jsval& ret)
{
	const JSClass *klass = JS_GetClass(obj);
	unsigned int slots = JSCLASS_RESERVED_SLOTS(klass);
	if (idx >= slots)
		return false;

	ret = JS_GetReservedSlot(obj, idx);

	return true;
}

js_proxy_t* jsb_new_proxy(void* nativeObj, JSObject* jsObj)
{
	js_proxy_t* p = nullptr;
	JS_NEW_PROXY(p, nativeObj, jsObj);
	return p;
}

js_proxy_t* jsb_get_native_proxy(void* nativeObj)
{
	js_proxy_t* p = nullptr;
	JS_GET_PROXY(p, nativeObj);
	return p;
}

js_proxy_t* jsb_get_js_proxy(JSObject* jsObj)
{
	js_proxy_t* p = nullptr;
	JS_GET_NATIVE_PROXY(p, jsObj);
	return p;
}

void jsb_remove_proxy(js_proxy_t* nativeProxy, js_proxy_t* jsProxy)
{
	JS_REMOVE_PROXY(nativeProxy, jsProxy);
}


