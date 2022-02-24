#include <nctl/Utf8.h>

namespace nctl {

const char *Utf8::utf8ToCodePoint(const char *substring, unsigned int &codePoint, unsigned int *codeUnits)
{
	if (substring == nullptr || *substring == '\0')
		return substring;

	unsigned char sequence[4] = { '\0', '\0', '\0', '\0' };
	sequence[0] = *substring;
	// Plain ASCII
	if (sequence[0] < 0x80)
	{
		codePoint = sequence[0];
		if (codeUnits)
			*codeUnits = sequence[0];
		return substring + 1;
	}

	// Four code units sequence
	if (sequence[0] >= 0xf0)
	{
		for (unsigned int i = 1; i < 4; i++)
		{
			sequence[i] = substring[i];
			if (sequence[i] < 0x80)
			{
				codePoint = InvalidUnicode;
				if (codeUnits)
					*codeUnits = InvalidUtf8;
				return substring + i;
			}
		}

		codePoint = ((sequence[0] - 0xf0) << 18) | ((sequence[1] - 0x80) << 12) | ((sequence[2] - 0x80) << 6) | (sequence[3] - 0x80);
		if (codeUnits)
			*codeUnits = (sequence[0] << 24) | (sequence[1] << 16) | (sequence[2] << 8) | sequence[3];
		return substring + 4;
	}
	// Three code units sequence
	else if (sequence[0] >= 0xe0)
	{
		for (unsigned int i = 1; i < 3; i++)
		{
			sequence[i] = substring[i];
			if (sequence[i] < 0x80)
			{
				codePoint = InvalidUnicode;
				if (codeUnits)
					*codeUnits = InvalidUtf8;
				return substring + i;
			}
		}

		codePoint = ((sequence[0] - 0xe0) << 12) | ((sequence[1] - 0x80) << 6) | (sequence[2] - 0x80);
		if (codeUnits)
			*codeUnits = (sequence[0] << 16) | (sequence[1] << 8) | sequence[2];
		return substring + 3;
	}
	// Two code units sequence
	else if (sequence[0] >= 0xc0)
	{
		sequence[1] = substring[1];
		if (sequence[1] < 0x80)
		{
			codePoint = InvalidUnicode;
			if (codeUnits)
				*codeUnits = InvalidUtf8;
			return substring + 1;
		}

		codePoint = ((sequence[0] - 0xc0) << 6) | (sequence[1] - 0x80);
		if (codeUnits)
			*codeUnits = (sequence[0] << 8) | sequence[1];
		return substring + 2;
	}
	else
	{
		codePoint = InvalidUnicode;
		if (codeUnits)
			*codeUnits = InvalidUtf8;
		return substring + 1;
	}
}

const char *Utf8::utf8ToCodePoint(const char *substring, unsigned int &codePoint)
{
	return utf8ToCodePoint(substring, codePoint, nullptr);
}

int Utf8::codePointToUtf8(unsigned int codePoint, char *substring, unsigned int *codeUnits)
{
	if (substring == nullptr)
		return 0;

	// Plain ASCII
	if (codePoint <= 0x7F)
	{
		substring[0] = static_cast<char>(codePoint);
		substring[1] = '\0';
		if (codeUnits)
			*codeUnits = substring[0];
		return 1;
	}
	// Two code units sequence
	else if (codePoint <= 0x07ff)
	{
		substring[0] = static_cast<char>(((codePoint >> 6) & 0x1f) | 0xc0);
		substring[1] = static_cast<char>(((codePoint >> 0) & 0x3f) | 0x80);
		substring[2] = '\0';

		if (codeUnits)
			*codeUnits = (static_cast<unsigned char>(substring[0]) << 8) | static_cast<unsigned char>(substring[1]);
		return 2;
	}
	// Three code units sequence
	else if (codePoint <= 0xffff)
	{
		substring[0] = static_cast<char>(((codePoint >> 12) & 0x0f) | 0xe0);
		substring[1] = static_cast<char>(((codePoint >>  6) & 0x3f) | 0x80);
		substring[2] = static_cast<char>(((codePoint >>  0) & 0x3f) | 0x80);
		substring[3] = '\0';
		if (codeUnits)
		{
			*codeUnits = (static_cast<unsigned char>(substring[0]) << 16) | (static_cast<unsigned char>(substring[1]) << 8) |
			             static_cast<unsigned char>(substring[2]);
		}
		return 3;
	}
	// Four code units sequence
	else if (codePoint <= 0x10ffff)
	{
		substring[0] = static_cast<char>(((codePoint >> 18) & 0x07) | 0xf0);
		substring[1] = static_cast<char>(((codePoint >> 12) & 0x3f) | 0x80);
		substring[2] = static_cast<char>(((codePoint >>  6) & 0x3f) | 0x80);
		substring[3] = static_cast<char>(((codePoint >>  0) & 0x3f) | 0x80);
		substring[4] = '\0';
		if (codeUnits)
		{
			*codeUnits = (static_cast<unsigned char>(substring[0]) << 24) | (static_cast<unsigned char>(substring[1]) << 16) |
			             (static_cast<unsigned char>(substring[2]) << 8) | static_cast<unsigned char>(substring[3]);
		}
		return 4;
	}
	// Invalid code point, returning the replacement character
	else
	{
		substring[0] = static_cast<char>(0xef);
		substring[1] = static_cast<char>(0xbf);
		substring[2] = static_cast<char>(0xbd);
		substring[3] = '\0';
		if (codeUnits)
		{
			*codeUnits = (static_cast<unsigned char>(substring[0]) << 16) | (static_cast<unsigned char>(substring[1]) << 8) |
			             static_cast<unsigned char>(substring[2]);
		}
		return 0;
	}
}

}
