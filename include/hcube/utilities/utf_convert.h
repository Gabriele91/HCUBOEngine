#pragma once
#include <hcube/config.h>
#include <string>

namespace std
{
	using u8string = std::string;
}

namespace hcube
{
	////////////////////////////////////////////////////////////////////////
	//utf
	HCUBE_API std::u8string to_utf8(const std::u16string &s);

	HCUBE_API std::u8string to_utf8(const std::u32string &s);

	HCUBE_API std::u16string to_utf16(const std::u8string &s);

	HCUBE_API std::u16string to_utf16(const std::u32string &s);

	HCUBE_API std::u32string to_utf32(const std::u8string &s);

	HCUBE_API std::u32string to_utf32(const std::u16string &s);
	////////////////////////////////////////////////////////////////////////
	//ucs
	HCUBE_API std::u16string to_ucs2(const std::u8string &s);

	HCUBE_API std::u32string to_ucs4(const std::u8string &s);

	HCUBE_API std::u32string to_ucs4(const std::u16string &s);

}
