#pragma once

#include <string>

typedef unsigned long long ull_t;
typedef std::string b93_t;
typedef std::string str_t;

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_64(x) bswap64(x)
#endif

#elif defined(__XTENSA__)

#include <machine/endian.h>

#define bswap_64(x) __bswap64(x)

#else

#include <byteswap.h>

#endif

class Base93
{
public:

	//Pad zero chars ('!') to string begin until its length
	//is divisible y 10. It's assumed that previous 10-char
	//numbers are also padded
	static b93_t pad93(const b93_t& str)
	{
		b93_t r = str;

		while (r.length() % 10 != 0)
		{
			r.insert(r.begin(), '!'); //Add lowest char to the end
		}
		return r;
	}

	//Add zero chars until input length is divisible by 8
	static str_t padUlong(const str_t& str)
	{
		str_t r = str;

		while (r.length() % 8 != 0)
		{
			r += '\0';
		}
		return r;
	}

	//Convert INT64 to B93 string, 10-chars padded
	static b93_t fromUlong(ull_t ull)
	{
		b93_t r;
		ull_t n = ull;

		while (n >= 93)
		{
			r.insert(r.begin(), (n % 93) + '!');
			n /= 93;
		}
		r.insert(r.begin(), (const char) (n + '!'));
		return pad93(r); //Pad zero chars on the left
	}

	//Convert a 10-char B93 string to INT64
	static ull_t toUlong(const b93_t &b)
	{
		if (b.length() > 10) return 0;

		ull_t nLevel = 1;
		ull_t r = 0;
		b93_t tmp = b;
		b93_t::const_reverse_iterator iter = tmp.crbegin();

		while (iter != tmp.crend())
		{
			char c = *iter;

			if (c < '!' || c > '~') return 0;
			r += ((ull_t)c - '!') * nLevel;
			nLevel *= 93;
			iter++;
		}
		return r;
	}

	//Swap bytes depending on CPU's byte-order
	static ull_t bytesSwap(ull_t ul, bool bBigEndian = true)
	{
		const union {
			unsigned short i;
			char c[2];
		} ichk{0x0102};
		
		return (ichk.c[0] == (bBigEndian ? 1 : 2)) ? ul : bswap_64(ul);
	}

	static b93_t fromString(const str_t &str)
	{
		b93_t r;
		b93_t sTemp = padUlong(str);
		const char* cszTemp = sTemp.c_str(); //Input buffer to scroll in
		size_t nNums = sTemp.length() / 8;

		while (nNums)
		{
			ull_t nNum = bytesSwap(*(const ull_t*)cszTemp);

			r += fromUlong(nNum);
			cszTemp += 8;
			nNums--;
		}
		return r;
	}

	static str_t toString(const b93_t &str)
	{
		str_t r;

		if (str.length() % 10 == 0)
		{
			b93_t s1;
			ull_t v;
			char szNum[9];

			szNum[8] = 0;
			for (int n = 0; n < str.length(); n += 10)
			{
				s1 = str.substr(n, 10);
				v = toUlong(s1);  //Convert B93 number to UINT64
				v = bytesSwap(v); //Previous number is Big-endian, so swap bytes
				                  //if current architechture is Little-endian
				std::memcpy(szNum, (const char*)&v, 8);
				r += szNum;
			}
		}
		return r;
	}

};

