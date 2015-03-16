#include "js_manual_conversions.h"
#include "../jsdebugger.h"
#include "ConvertUTF.h"
#include "../global_define.h"
#include <locale>
#include <codecvt>

#define CC_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
// JSStringWrapper
JSStringWrapper::JSStringWrapper()
	: _buffer(nullptr)
{
}

JSStringWrapper::JSStringWrapper(JSString* str, JSContext* cx/* = NULL*/)
	: _buffer(nullptr)
{
	set(str, cx);
}

JSStringWrapper::JSStringWrapper(jsval val, JSContext* cx/* = NULL*/)
	: _buffer(nullptr)
{
	set(val, cx);
}

JSStringWrapper::~JSStringWrapper()
{
	CC_SAFE_DELETE_ARRAY(_buffer);
}

void JSStringWrapper::set(jsval val, JSContext* cx)
{
	if (val.isString())
	{
		this->set(val.toString(), cx);
	}
	else
	{
		CC_SAFE_DELETE_ARRAY(_buffer);
	}
}

int wcslen(const unsigned short* str)
{
	if (str == nullptr)
		return -1;
	int i = 0;
	while (*str++) i++;
	return i;
}

bool UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16)
{
	if (utf8.empty())
	{
		outUtf16.clear();
		return true;
	}

	bool ret = false;

	const size_t utf16Bytes = (utf8.length() + 1) * sizeof(char16_t);
	char16_t* utf16 = (char16_t*)malloc(utf16Bytes);
	memset(utf16, 0, utf16Bytes);

	char* utf16ptr = reinterpret_cast<char*>(utf16);
	const UTF8* error = nullptr;

	if (llvm::ConvertUTF8toWide(2, utf8, utf16ptr, error))
	{
		outUtf16 = utf16;
		ret = true;
	}

	free(utf16);

	return ret;
}

bool UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8)
{
	if (utf16.empty())
	{
		outUtf8.clear();
		return true;
	}

	return llvm::convertUTF16ToUTF8String(utf16, outUtf8);
}

unsigned short* utf8_to_utf16(const char* str_old, int length/* = -1*/, int* rUtf16Size/* = nullptr*/)
{
	if (str_old == nullptr)
		return nullptr;

	unsigned short* ret = nullptr;

	std::u16string outUtf16;
	if (length < 0)
		length = strlen(str_old);
	std::string inUtf8;
	inUtf8.append(str_old, length);
	bool succeed = UTF8ToUTF16(inUtf8, outUtf16);

	if (succeed)
	{
		ret = new unsigned short[outUtf16.length() + 1];
		ret[outUtf16.length()] = 0;
		memcpy(ret, outUtf16.data(), outUtf16.length() * sizeof(unsigned short));
		if (rUtf16Size)
		{
			*rUtf16Size = static_cast<int>(outUtf16.length());
		}
	}

	return ret;
}

char * utf16_to_utf8(const unsigned short  *str,
	int             len,
	long            *items_read,
	long            *items_written)
{
	if (str == nullptr)
		return nullptr;


	std::u16string utf16;
	int utf16Len = len < 0 ? wcslen(str) : len;

	for (int i = 0; i < utf16Len; ++i)
	{
		utf16.push_back(str[i]);
	}

	char* ret = nullptr;
	std::string outUtf8;
	bool succeed = UTF16ToUTF8(utf16, outUtf8);

	if (succeed)
	{
		ret = new char[outUtf8.length() + 1];
		ret[outUtf8.length()] = '\0';
		memcpy(ret, outUtf8.data(), outUtf8.length());
	}

	return ret;
}

void JSStringWrapper::set(JSString* str, JSContext* cx)
{
	CC_SAFE_DELETE_ARRAY(_buffer);

	if (!cx)
	{
		cx = jsdebugger::getInstance()->getGlobalContext();
	}
	// JS_EncodeString isn't supported in SpiderMonkey ff19.0.
	//buffer = JS_EncodeString(cx, string);
	unsigned short* pStrUTF16 = (unsigned short*)JS_GetStringCharsZ(cx, str);

	_buffer = utf16_to_utf8(pStrUTF16, -1, NULL, NULL);
}

const char* JSStringWrapper::get()
{
	return _buffer ? _buffer : "";
}

bool jsval_to_std_string(JSContext *cx, jsval v, std::string* ret) {
	JSString *tmp = JS::ToString(cx, JS::RootedValue(cx, v));
	JSB_PRECONDITION3(tmp, cx, false, "Error processing arguments");

	JSStringWrapper str(tmp);
	*ret = str.get();
	return true;
}

bool jsval_to_std_wstring(JSContext* cx, jsval v, std::wstring* ret){

	std::string tmp;
	jsval_to_std_string(cx, v, &tmp);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	*ret = converter.from_bytes(tmp.c_str());
	return true;
}



jsval c_string_to_jsval(JSContext* cx, const char* v, size_t length )
{
	if (v == NULL)
	{
		return JSVAL_NULL;
	}
	if (length == 0)
	{
		length = strlen(v);
	}

	JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

		if (0 == length)
		{
		auto emptyStr = JS_NewStringCopyZ(cx, "");
		return STRING_TO_JSVAL(emptyStr);
		}

	jsval ret = JSVAL_NULL;
	int utf16_size = 0;
	jschar* strUTF16 = (jschar*)utf8_to_utf16(v, (int)length, &utf16_size);

	if (strUTF16 && utf16_size > 0) {
		JSString* str = JS_NewUCStringCopyN(cx, strUTF16, utf16_size);
		if (str) {
			ret = STRING_TO_JSVAL(str);
		}
		delete[] strUTF16;
	}
	return ret;
}

jsval std_string_to_jsval(JSContext* cx, const std::string& v)
{
	return c_string_to_jsval(cx, v.c_str(), v.size());
}

jsval std_vector_string_to_jsval(JSContext *cx, const std::vector<std::string>& v)
{
	JSObject *jsretArr = JS_NewArrayObject(cx, 0, NULL);

	int i = 0;
	for (const std::string obj : v)
	{
		JS::RootedValue arrElement(cx);
		arrElement = std_string_to_jsval(cx, obj);

		if (!JS_SetElement(cx, jsretArr, i, &arrElement)) {
			break;
		}
		++i;
	}
	return OBJECT_TO_JSVAL(jsretArr);
}

bool jsval_to_std_vector_string(JSContext *cx, jsval vp, std::vector<std::string>* ret)
{
	JS::RootedObject jsobj(cx);
	bool ok = vp.isObject() && JS_ValueToObject(cx, JS::RootedValue(cx, vp), &jsobj);
	JSB_PRECONDITION3(ok, cx, false, "Error converting value to object");
	JSB_PRECONDITION3(jsobj && JS_IsArrayObject(cx, jsobj), cx, false, "Object must be an array");

	uint32_t len = 0;
	JS_GetArrayLength(cx, jsobj, &len);

	for (uint32_t i = 0; i < len; i++)
	{
		JS::RootedValue value(cx);
		if (JS_GetElement(cx, jsobj, i, &value))
		{
			if (JSVAL_IS_STRING(value))
			{
				JSStringWrapper valueWapper(JSVAL_TO_STRING(value), cx);
				ret->push_back(valueWapper.get());
			}
			else
			{
				JS_ReportError(cx, "not supported type in array");
				return false;
			}
		}
	}

	return true;
}

jsval long_to_jsval(JSContext *cx, long number)
{
#ifdef __LP64__
	assert(sizeof(long) == 8);

	char chr[128];
	snprintf(chr, sizeof(chr) - 1, "%ld", number);
	JSString *ret_obj = JS_NewStringCopyZ(cx, chr);
	return STRING_TO_JSVAL(ret_obj);
#else
	CCASSERT(sizeof(int) == 4, "Error!");
	return INT_TO_JSVAL(number);
#endif
}

jsval wchar_to_jsval(JSContext* cx, unsigned short* v, size_t length)
{
	if (v == NULL )
	{
		return JSVAL_NULL;
	}

	if (length == 0)
		length = wcslen(v);

	JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

		if (0 == length)
		{
		auto emptyStr = JS_NewStringCopyZ(cx, "");
		return STRING_TO_JSVAL(emptyStr);
		}

	jsval ret = JSVAL_NULL;
	int utf16_size = length;
	jschar* strUTF16 = (jschar*)v;

	if (strUTF16 && utf16_size > 0) {
		JSString* str = JS_NewUCStringCopyN(cx, strUTF16, utf16_size);
		if (str) {
			ret = STRING_TO_JSVAL(str);
		}
	}
	return ret;
}

jsval std_wstring_to_jsval(JSContext* cx, const std::wstring& v)
{
	return wchar_to_jsval(cx, (unsigned short*)v.c_str(), v.size());
}
