#pragma once
/**
	@file
	@brief functions for T[]
	@author MITSUNARI Shigeo(@herumi)
	@license modified new BSD license
	http://opensource.org/licenses/BSD-3-Clause
*/
#include <cybozu/bit_operation.hpp>

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4456)
	#pragma warning(disable : 4459)
#endif

namespace mcl { namespace fp {

/*
	get pp such that p * pp = -1 mod M,
	where p is prime and M = 1 << 64(or 32).
	@param pLow [in] p mod M
*/
template<class T>
T getMontgomeryCoeff(T pLow)
{
	T ret = 0;
	T t = 0;
	T x = 1;
	for (size_t i = 0; i < sizeof(T) * 8; i++) {
		if ((t & 1) == 0) {
			t += pLow;
			ret += x;
		}
		t >>= 1;
		x <<= 1;
	}
	return ret;
}

template<class T>
int compareArray(const T* x, const T* y, size_t n)
{
	for (size_t i = n - 1; i != size_t(-1); i--) {
		T a = x[i];
		T b = y[i];
		if (a != b) return a < b ? -1 : 1;
	}
	return 0;
}

template<class T>
bool isLessArray(const T *x, const T* y, size_t n)
{
	for (size_t i = n - 1; i != size_t(-1); i--) {
		T a = x[i];
		T b = y[i];
		if (a != b) return a < b;
	}
	return false;
}

template<class T>
bool isGreaterOrEqualArray(const T *x, const T* y, size_t n)
{
	return !isLessArray(x, y, n);
}

template<class T>
bool isLessOrEqualArray(const T *x, const T* y, size_t n)
{
	for (size_t i = n - 1; i != size_t(-1); i--) {
		T a = x[i];
		T b = y[i];
		if (a != b) return a < b;
	}
	return true;
}

template<class T>
bool isGreaterArray(const T *x, const T* y, size_t n)
{
	return !isLessOrEqualArray(x, y, n);
}

template<class T>
bool isEqualArray(const T* x, const T* y, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		if (x[i] != y[i]) return false;
	}
	return true;
}

template<class T>
bool isZeroArray(const T *x, size_t n)
{
	for (size_t i = 0; i < n; i++) {
		if (x[i]) return false;
	}
	return true;
}

template<class T>
void clearArray(T *x, size_t begin, size_t end)
{
	for (size_t i = begin; i < end; i++) x[i] = 0;
}

template<class T>
void copyArray(T *y, const T *x, size_t n)
{
	for (size_t i = 0; i < n; i++) y[i] = x[i];
}

/*
	x &= (1 << bitSize) - 1
*/
template<class T>
void maskArray(T *x, size_t n, size_t bitSize)
{
	const size_t TbitSize = sizeof(T) * 8;
	assert(bitSize <= TbitSize * n);
	const size_t q = bitSize / TbitSize;
	const size_t r = bitSize % TbitSize;
	if (r) {
		x[q] &= (T(1) << r) - 1;
		clearArray(x, q + 1, n);
	} else {
		clearArray(x, q, n);
	}
}

/*
	return non zero size of x[]
	return 1 if x[] == 0
*/
template<class T>
size_t getNonZeroArraySize(const T *x, size_t n)
{
	assert(n > 0);
	while (n > 0) {
		if (x[n - 1]) return n;
		n--;
	}
	return 1;
}

/*
	@param out [inout] : set element of G ; out = x^y[]
	@param x [in]
	@param y [in]
	@param n [in] size of y[]
	@param limitBit [in] const time version if the value is positive
	@note &out != x and out = the unit element of G
*/
template<class G, class Mul, class Sqr, class T>
void powGeneric(G& out, const G& x, const T *y, size_t n, const Mul& mul, const Sqr& sqr, void normalize(G&, const G&), size_t limitBit = 0)
{
	assert(&out != &x);
	G tbl[4]; // tbl = { discard, x, x^2, x^3 }
	T v;
	bool constTime = limitBit > 0;
	int maxBit = 0;
	int m = 0;
	while (n > 0) {
		if (y[n - 1]) break;
		n--;
	}
	if (n == 0) {
		if (constTime) goto DummyLoop;
		return;
	}
	if (!constTime && n == 1) {
		switch (y[0]) {
		case 1:
			out = x;
			return;
		case 2:
			sqr(out, x);
			return;
		case 3:
			sqr(out, x);
			mul(out, out, x);
			return;
		case 4:
			sqr(out, x);
			sqr(out, out);
			return;
		}
	}
	if (normalize != 0) {
		normalize(tbl[0], x);
	} else {
		tbl[0] = x;
	}
	tbl[1] = tbl[0];
	sqr(tbl[2], tbl[1]);
	if (normalize != 0) { normalize(tbl[2], tbl[2]); }
	mul(tbl[3], tbl[2], x);
	if (normalize != 0) { normalize(tbl[3], tbl[3]); }
	v = y[n - 1];
	assert(v);
	m = cybozu::bsr<T>(v);
	maxBit = int(m + (n - 1) * sizeof(T) * 8);
	if (m & 1) {
		m--;
		T idx = (v >> m) & 3;
		assert(idx > 0);
		out = tbl[idx];
	} else {
		out = x;
	}
	for (int i = (int)n - 1; i >= 0; i--) {
		T v = y[i];
		for (int j = m - 2; j >= 0; j -= 2) {
			sqr(out, out);
			sqr(out, out);
			T idx = (v >> j) & 3;
			if (idx == 0) {
				if (constTime) mul(tbl[0], tbl[0], tbl[1]);
			} else {
				mul(out, out, tbl[idx]);
			}
		}
		m = (int)sizeof(T) * 8;
	}
DummyLoop:
	if (!constTime) return;
	G D = out;
	for (size_t i = maxBit + 1; i < limitBit; i += 2) {
		sqr(D, D);
		sqr(D, D);
		mul(D, D, tbl[1]);
	}
}

/*
	shortcut of multiplication by Unit
*/
template<class T, class U>
bool mulSmallUnit(T& z, const T& x, U y)
{
	switch (y) {
	case 0: z.clear(); break;
	case 1: z = x; break;
	case 2: T::add(z, x, x); break;
	case 3: { T t; T::add(t, x, x); T::add(z, t, x); break; }
	case 4: T::add(z, x, x); T::add(z, z, z); break;
	case 5: { T t; T::add(t, x, x); T::add(t, t, t); T::add(z, t, x); break; }
	case 6: { T t; T::add(t, x, x); T::add(t, t, x); T::add(z, t, t); break; }
	case 7: { T t; T::add(t, x, x); T::add(t, t, t); T::add(t, t, t); T::sub(z, t, x); break; }
	case 8: T::add(z, x, x); T::add(z, z, z); T::add(z, z, z); break;
	case 9: { T t; T::add(t, x, x); T::add(t, t, t); T::add(t, t, t); T::add(z, t, x); break; }
	case 10: { T t; T::add(t, x, x); T::add(t, t, t); T::add(t, t, x); T::add(z, t, t); break; }
	default:
		return false;
	}
	return true;
}

} } // mcl::fp

#ifdef _MSC_VER
	#pragma warning(pop)
#endif
