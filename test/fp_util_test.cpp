#define PUT(x) std::cout << #x "=" << (x) << std::endl
#include "../src/conversion.hpp"
#include <cybozu/test.hpp>
#include <mcl/gmp_util.hpp>
#include <mcl/fp.hpp>

CYBOZU_TEST_AUTO(toStr16)
{
	const struct {
		uint32_t x[4];
		size_t n;
		const char *str;
	} tbl[] = {
		{ { 0, 0, 0, 0 }, 0, "0" },
		{ { 0x123, 0, 0, 0 }, 1, "123" },
		{ { 0x12345678, 0xaabbcc, 0, 0 }, 2, "aabbcc12345678" },
		{ { 0, 0x12, 0x234a, 0 }, 3, "234a0000001200000000" },
		{ { 1, 2, 0xffffffff, 0x123abc }, 4, "123abcffffffff0000000200000001" },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		std::string str;
		mcl::fp::toStr16(str, tbl[i].x, tbl[i].n, false);
		CYBOZU_TEST_EQUAL(str, tbl[i].str);
		mcl::fp::toStr16(str, tbl[i].x, tbl[i].n, true);
		CYBOZU_TEST_EQUAL(str, std::string("0x") + tbl[i].str);
	}
}

// CYBOZU_TEST_AUTO(toStr2) // QQQ
// CYBOZU_TEST_AUTO(verifyStr) // QQQ

CYBOZU_TEST_AUTO(fromStr16)
{
	const struct {
		const char *str;
		uint64_t x[4];
	} tbl[] = {
		{ "0", { 0, 0, 0, 0 } },
		{ "5", { 5, 0, 0, 0 } },
		{ "123", { 0x123, 0, 0, 0 } },
		{ "123456789012345679adbc", { uint64_t(0x789012345679adbcull), 0x123456, 0, 0 } },
		{ "ffffffff26f2fc170f69466a74defd8d", { uint64_t(0x0f69466a74defd8dull), uint64_t(0xffffffff26f2fc17ull), 0, 0 } },
		{ "100000000000000000000000000000033", { uint64_t(0x0000000000000033ull), 0, 1, 0 } },
		{ "11ee12312312940000000000000000000000000002342343", { uint64_t(0x0000000002342343ull), uint64_t(0x0000000000000000ull), uint64_t(0x11ee123123129400ull), 0 } },
		{ "1234567890abcdefABCDEF123456789aba32134723424242424", { uint64_t(0x2134723424242424ull), uint64_t(0xDEF123456789aba3ull), uint64_t(0x4567890abcdefABCull), 0x123 } },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		const size_t xN = 4;
		uint64_t x[xN];
		mcl::fp::fromStr16(x, xN, tbl[i].str, strlen(tbl[i].str));
		for (size_t j = 0; j < xN; j++) {
			CYBOZU_TEST_EQUAL(x[j], tbl[i].x[j]);
		}
	}
}

CYBOZU_TEST_AUTO(compareArray)
{
	const struct {
		uint32_t a[4];
		uint32_t b[4];
		size_t n;
		int expect;
	} tbl[] = {
		{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, 0 },
		{ { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, 1, 1 },
		{ { 0, 0, 0, 0 }, { 1, 0, 0, 0 }, 1, -1 },
		{ { 1, 0, 0, 0 }, { 1, 0, 0, 0 }, 1, 0 },
		{ { 3, 1, 1, 0 }, { 2, 1, 1, 0 }, 4, 1 },
		{ { 9, 2, 1, 1 }, { 1, 3, 1, 1 }, 4, -1 },
		{ { 1, 7, 8, 4 }, { 1, 7, 8, 9 }, 3, 0 },
		{ { 1, 7, 8, 4 }, { 1, 7, 8, 9 }, 4, -1 },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		int e = mcl::fp::compareArray(tbl[i].a, tbl[i].b, tbl[i].n);
		CYBOZU_TEST_EQUAL(e, tbl[i].expect);
	}
}

CYBOZU_TEST_AUTO(isLessArray)
{
	const struct {
		uint32_t a[4];
		uint32_t b[4];
		size_t n;
		bool expect;
	} tbl[] = {
		{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, false },
		{ { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, 1, false },
		{ { 0, 0, 0, 0 }, { 1, 0, 0, 0 }, 1, true },
		{ { 1, 0, 0, 0 }, { 1, 0, 0, 0 }, 1, false },
		{ { 3, 1, 1, 0 }, { 2, 1, 1, 0 }, 4, false },
		{ { 3, 1, 2, 0 }, { 2, 2, 2, 0 }, 4, true },
		{ { 9, 2, 1, 1 }, { 1, 3, 1, 1 }, 4, true },
		{ { 1, 7, 8, 4 }, { 1, 7, 8, 9 }, 3, false },
		{ { 1, 7, 8, 4 }, { 1, 7, 8, 9 }, 4, true },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		bool e = mcl::fp::isLessArray(tbl[i].a, tbl[i].b, tbl[i].n);
		CYBOZU_TEST_EQUAL(e, tbl[i].expect);
		e = mcl::fp::isGreaterArray(tbl[i].b, tbl[i].a, tbl[i].n);
		CYBOZU_TEST_EQUAL(e, tbl[i].expect);
	}
}

CYBOZU_TEST_AUTO(isLessOrEqualArray)
{
	const struct {
		uint32_t a[4];
		uint32_t b[4];
		size_t n;
		bool expect;
	} tbl[] = {
		{ { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, true },
		{ { 1, 0, 0, 0 }, { 0, 0, 0, 0 }, 1, false },
		{ { 0, 0, 0, 0 }, { 1, 0, 0, 0 }, 1, true },
		{ { 1, 0, 0, 0 }, { 1, 0, 0, 0 }, 1, true },
		{ { 3, 1, 1, 0 }, { 2, 1, 1, 0 }, 4, false },
		{ { 3, 1, 2, 0 }, { 2, 2, 2, 0 }, 4, true },
		{ { 9, 2, 1, 1 }, { 1, 3, 1, 1 }, 4, true },
		{ { 1, 7, 8, 4 }, { 1, 7, 8, 9 }, 3, true },
		{ { 1, 7, 8, 4 }, { 1, 7, 8, 9 }, 4, true },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		bool e = mcl::fp::isLessOrEqualArray(tbl[i].a, tbl[i].b, tbl[i].n);
		CYBOZU_TEST_EQUAL(e, tbl[i].expect);
		e = mcl::fp::isGreaterOrEqualArray(tbl[i].b, tbl[i].a, tbl[i].n);
		CYBOZU_TEST_EQUAL(e, tbl[i].expect);
	}
}

struct Rand {
	std::vector<uint32_t> v;
	const uint8_t *p;
	size_t pos;
	size_t endPos;
	void read(void *x, size_t n)
	{
		if (pos + n > endPos) throw cybozu::Exception("Rand:get:bad n") << pos << n << endPos;
		uint8_t *dst = (uint8_t*)x;
		memcpy(dst, p + pos, n);
		pos += n;
	}
	uint32_t operator()()
	{
		char buf[4];
		read(buf, 4);
		uint32_t v;
		memcpy(&v, buf, 4);
		return v;
	}
	Rand(const uint32_t *x, size_t n)
		: p(0)
		, pos(0)
	{
		for (size_t i = 0; i < n; i++) {
			v.push_back(x[i]);
		}
		p = (uint8_t*)&v[0];
		endPos = v.size() * 4;
	}
};

CYBOZU_TEST_AUTO(getRandVal)
{
	const size_t rn = 8;
	const struct {
		uint32_t r[rn];
		uint32_t mod[2];
		size_t bitSize;
		uint32_t expect[2];
	} tbl[] = {
		{ { 1, 2, 3, 4, 5, 6, 7, 8 }, { 5, 6 }, 64, { 1, 2 } },
		{ { 0xfffffffc, 0x7, 3, 4, 5, 6, 7, 8 }, { 0xfffffffe, 0x3 }, 34, { 0xfffffffc, 0x3 } },
		{ { 0xfffffffc, 0x7, 3, 4, 5, 6, 7, 8 }, { 0xfffffffb, 0x3 }, 34, { 3, 0 } },
		{ { 2, 3, 5, 7, 4, 3, 0, 3 }, { 1, 0x3 }, 34, { 0, 3 } },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		Rand rg(tbl[i].r, rn);
#if CYBOZU_OS_BIT == 64
		uint64_t out[1];
#else
		uint32_t out[2];
#endif
		mcl::fp::RandGen wrg(rg);
#if CYBOZU_OS_BIT == 64
		uint64_t mod = tbl[i].mod[0] | (uint64_t(tbl[i].mod[1]) << 32);
		mcl::fp::getRandVal(out, wrg, &mod, tbl[i].bitSize);
		uint64_t expect = tbl[i].expect[0] | (uint64_t(tbl[i].expect[1]) << 32);
		CYBOZU_TEST_EQUAL(out[0], expect);
#else
		mcl::fp::getRandVal(out, wrg, tbl[i].mod, tbl[i].bitSize);
		CYBOZU_TEST_EQUAL(out[0], tbl[i].expect[0]);
		CYBOZU_TEST_EQUAL(out[1], tbl[i].expect[1]);
#endif
	}
}

CYBOZU_TEST_AUTO(maskArray)
{
#if 1
	const size_t n = 2;
	uint32_t org[n] = { 0xabce1234, 0xffffef32 };
	for (size_t i = 0; i <= sizeof(org) * 8; i++) {
		uint32_t x[n];
		memcpy(x, org, sizeof(org));
		mcl::fp::maskArray(x, n, i);
		mpz_class t;
		mcl::gmp::setArray(t, org, n);
		t &= (mpz_class(1) << i) - 1;
		uint32_t y[n];
		mcl::gmp::getArray(y, n, t);
		CYBOZU_TEST_EQUAL_ARRAY(x, y, n);
	}
#else
	const size_t n = 4;
	uint16_t org[n] = { 0x1234, 0xabce, 0xef32, 0xffff };
	for (size_t i = 0; i <= sizeof(org) * 8; i++) {
		uint16_t x[n];
		memcpy(x, org, sizeof(org));
		mcl::fp::maskArray(x, n, i);
		mpz_class t;
		mcl::gmp::setArray(t, org, n);
		t &= (mpz_class(1) << i) - 1;
		uint16_t y[n];
		mcl::gmp::getArray(y, n, t);
		CYBOZU_TEST_EQUAL_ARRAY(x, y, n);
	}
#endif
}

CYBOZU_TEST_AUTO(stream)
{
	const char *nulTbl[] = { "", "    ", " \t\t\n\n  " };
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(nulTbl); i++) {
		const char *p = nulTbl[i];
		cybozu::MemoryInputStream is(p, strlen(p));
		std::string w = "abc";
		mcl::fp::local::loadWord(w, is);
		CYBOZU_TEST_ASSERT(w.empty());
	}
	const struct {
		const char *buf;
		const char *expect[2];
		size_t n;
	} tbl[] = {
		{ "\t\t \n\rabc\r\r\n    def", { "abc", "def" }, 2 },
		{ "123", { "123" }, 1 },
		{ "123\n", { "123" }, 1 },
		{ "123 456", { "123", "456" }, 2 },
	};
	for (size_t i = 0; i < CYBOZU_NUM_OF_ARRAY(tbl); i++) {
		const char *buf = tbl[i].buf;
		{
			cybozu::MemoryInputStream is(buf, strlen(buf));
			for (size_t j = 0; j < tbl[i].n; j++) {
				std::string w;
				mcl::fp::local::loadWord(w, is);
				CYBOZU_TEST_EQUAL(w, tbl[i].expect[j]);
			}
		}
		{
			std::istringstream is(buf);
			for (size_t j = 0; j < tbl[i].n; j++) {
				std::string w;
				mcl::fp::local::loadWord(w, is);
				CYBOZU_TEST_EQUAL(w, tbl[i].expect[j]);
			}
		}
	}
}
