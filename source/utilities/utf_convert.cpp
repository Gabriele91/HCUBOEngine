#include <hcube/utilities/utf_convert.h>
#include <locale>
#include <codecvt>

namespace hcube
{
//work around for bug in MS Visual C++ 2015 https://social.msdn.microsoft.com/Forums/en-US/8f40dcd8-c67f-4eba-9134-a19b9178e481/vs-2015-rc-linker-stdcodecvt-error?forum=vcgeneral
#if _MSC_VER >= 1900 

	HCUBE_API std::u8string to_utf8(const std::u16string &s)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
		auto p = reinterpret_cast<const int16_t *>(s.data());
		return convert.to_bytes(p, p + s.size());
	}

	HCUBE_API std::u8string to_utf8(const std::u32string &s)
	{
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
		auto p = reinterpret_cast<const int32_t *>(s.data());
		return convert.to_bytes(p, p + s.size());
	}

	HCUBE_API std::u16string to_utf16(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<int16_t>, int16_t> convert;
		auto asInt = convert.from_bytes(s);
		return std::u16string(reinterpret_cast<char16_t const *>(asInt.data()), asInt.length());
	}

	HCUBE_API std::u16string to_utf16(const std::u32string &s)
	{
		return to_utf16(to_utf8(s));
	}

	HCUBE_API std::u32string to_utf32(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
		auto asInt = convert.from_bytes(s);
		return std::u32string(reinterpret_cast<char32_t const *>(asInt.data()), asInt.length());
	}

	HCUBE_API std::u32string to_utf32(const std::u16string &s)
	{
		return to_utf32(to_utf8(s));
	}

	HCUBE_API std::u16string to_ucs2(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<short>, short> convert;
		auto asShort = convert.from_bytes(s);
		return std::u16string(reinterpret_cast<char16_t const *>(asShort.data()), asShort.length());
	}

	HCUBE_API std::u32string to_ucs4(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> convert;
		auto asInt = convert.from_bytes(s);
		return std::u32string(reinterpret_cast<char32_t const *>(asInt.data()), asInt.length());
	}

	HCUBE_API std::u32string to_ucs4(const std::u16string &s)
	{
		return to_utf32(s);
	}

#else


	HCUBE_API std::u8string to_utf8(const std::u16string &s)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
		return conv.to_bytes(s);
	}

	HCUBE_API std::u8string to_utf8(const std::u32string &s)
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.to_bytes(s);
	}

	HCUBE_API std::u16string to_utf16(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
		return conv.from_bytes(s);
	}

	HCUBE_API std::u16string to_utf16(const std::u32string &s)
	{
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
		std::string bytes = conv.to_bytes(s);
		return std::u16string(reinterpret_cast<const char16_t*>(bytes.c_str()), bytes.length() / sizeof(char16_t));
	}

	HCUBE_API std::u32string to_utf32(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(s);
	}

	HCUBE_API std::u32string to_utf32(const std::u16string &s)
	{
		const char16_t *pData = s.c_str();
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
		return conv.from_bytes(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData + s.length()));
	}

	HCUBE_API std::u16string to_ucs2(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
		return convert.from_bytes(s.data());
	}

	HCUBE_API std::u32string to_ucs4(const std::u8string &s)
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(s);
	}

	HCUBE_API std::u32string to_ucs4(const std::u16string &s)
	{
		const char16_t *pData = s.c_str();
		std::wstring_convert<std::codecvt_utf16<char32_t>, char32_t> conv;
		return conv.from_bytes(reinterpret_cast<const char*>(pData), reinterpret_cast<const char*>(pData + s.length()));
	}

#endif
}