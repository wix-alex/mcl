#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <iosfwd>
#include <stdint.h>
#include <memory.h>
#include "../src/bn_c_impl.hpp"
#define MCLSHE_DLL_EXPORT

#include <mcl/she.h>
#include <mcl/she.hpp>

using namespace mcl::she;
using namespace mcl::bn_current;

#if defined(CYBOZU_CPP_VERSION) && CYBOZU_CPP_VERSION >= CYBOZU_CPP_VERSION_CPP11
#include <mutex>
	#define USE_STD_MUTEX
#else
#include <cybozu/mutex.hpp>
#endif

static SecretKey *cast(sheSecretKey *p) { return reinterpret_cast<SecretKey*>(p); }
static const SecretKey *cast(const sheSecretKey *p) { return reinterpret_cast<const SecretKey*>(p); }

static PublicKey *cast(shePublicKey *p) { return reinterpret_cast<PublicKey*>(p); }
static const PublicKey *cast(const shePublicKey *p) { return reinterpret_cast<const PublicKey*>(p); }

static PrecomputedPublicKey *cast(shePrecomputedPublicKey *p) { return reinterpret_cast<PrecomputedPublicKey*>(p); }
static const PrecomputedPublicKey *cast(const shePrecomputedPublicKey *p) { return reinterpret_cast<const PrecomputedPublicKey*>(p); }

static CipherTextG1 *cast(sheCipherTextG1 *p) { return reinterpret_cast<CipherTextG1*>(p); }
static const CipherTextG1 *cast(const sheCipherTextG1 *p) { return reinterpret_cast<const CipherTextG1*>(p); }

static CipherTextG2 *cast(sheCipherTextG2 *p) { return reinterpret_cast<CipherTextG2*>(p); }
static const CipherTextG2 *cast(const sheCipherTextG2 *p) { return reinterpret_cast<const CipherTextG2*>(p); }

static CipherTextGT *cast(sheCipherTextGT *p) { return reinterpret_cast<CipherTextGT*>(p); }
static const CipherTextGT *cast(const sheCipherTextGT *p) { return reinterpret_cast<const CipherTextGT*>(p); }

static ZkpBin *cast(sheZkpBin *p) { return reinterpret_cast<ZkpBin*>(p); }
static const ZkpBin *cast(const sheZkpBin *p) { return reinterpret_cast<const ZkpBin*>(p); }

int sheInit(int curve, int maxUnitSize)
	try
{
	if (maxUnitSize != MCLBN_FP_UNIT_SIZE) {
		fprintf(stderr, "err sheInit:maxUnitSize is mismatch %d %d\n", maxUnitSize, MCLBN_FP_UNIT_SIZE);
		return -1;
	}
#ifdef USE_STD_MUTEX
	static std::mutex m;
	std::lock_guard<std::mutex> lock(m);
#else
	static cybozu::Mutex m;
	cybozu::AutoLock lock(m);
#endif
	static int g_curve = -1;
	if (g_curve == curve) return 0;

	mcl::CurveParam cp;
	switch (curve) {
	case MCL_BN254:
		cp = mcl::BN254;
		break;
	case MCL_BN381_1:
		cp = mcl::BN381_1;
		break;
	case MCL_BN381_2:
		cp = mcl::BN381_2;
		break;
	case MCL_BN462:
		cp = mcl::BN462;
		break;
	case MCL_BN_SNARK1:
		cp = mcl::BN_SNARK1;
		break;
	case MCL_BLS12_381:
		cp = mcl::BLS12_381;
		break;
	default:
		fprintf(stderr, "err bad curve %d\n", curve);
		return -1;
	}
	SHE::init(cp);
	g_curve = curve;
	return 0;
} catch (std::exception& e) {
	fprintf(stderr, "err sheInit %s\n", e.what());
	return -1;
}

template<class T>
mclSize serialize(void *buf, mclSize maxBufSize, const T *x)
	try
{
	return cast(x)->serialize(buf, maxBufSize);
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return 0;
}

mclSize sheSecretKeySerialize(void *buf, mclSize maxBufSize, const sheSecretKey *sec)
{
	return serialize(buf, maxBufSize, sec);
}

mclSize shePublicKeySerialize(void *buf, mclSize maxBufSize, const shePublicKey *pub)
{
	return serialize(buf, maxBufSize, pub);
}

mclSize sheCipherTextG1Serialize(void *buf, mclSize maxBufSize, const sheCipherTextG1 *c)
{
	return serialize(buf, maxBufSize, c);
}

mclSize sheCipherTextG2Serialize(void *buf, mclSize maxBufSize, const sheCipherTextG2 *c)
{
	return serialize(buf, maxBufSize, c);
}

mclSize sheCipherTextGTSerialize(void *buf, mclSize maxBufSize, const sheCipherTextGT *c)
{
	return serialize(buf, maxBufSize, c);
}

mclSize sheZkpBinSerialize(void *buf, mclSize maxBufSize, const sheZkpBin *zkp)
{
	return serialize(buf, maxBufSize, zkp);
}

template<class T>
mclSize deserialize(T *x, const void *buf, mclSize bufSize)
	try
{
	return cast(x)->deserialize(buf, bufSize);
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return 0;
}

mclSize sheSecretKeyDeserialize(sheSecretKey* sec, const void *buf, mclSize bufSize)
{
	return deserialize(sec, buf, bufSize);
}

mclSize shePublicKeyDeserialize(shePublicKey* pub, const void *buf, mclSize bufSize)
{
	return deserialize(pub, buf, bufSize);
}

mclSize sheCipherTextG1Deserialize(sheCipherTextG1* c, const void *buf, mclSize bufSize)
{
	return deserialize(c, buf, bufSize);
}

mclSize sheCipherTextG2Deserialize(sheCipherTextG2* c, const void *buf, mclSize bufSize)
{
	return deserialize(c, buf, bufSize);
}

mclSize sheCipherTextGTDeserialize(sheCipherTextGT* c, const void *buf, mclSize bufSize)
{
	return deserialize(c, buf, bufSize);
}

mclSize sheZkpBinDeserialize(sheZkpBin* zkp, const void *buf, mclSize bufSize)
{
	return deserialize(zkp, buf, bufSize);
}

int sheSecretKeySetByCSPRNG(sheSecretKey *sec)
	try
{
	cast(sec)->setByCSPRNG();
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

void sheGetPublicKey(shePublicKey *pub, const sheSecretKey *sec)
{
	cast(sec)->getPublicKey(*cast(pub));
}

static int setRangeForDLP(void (*f)(mclSize), mclSize hashSize)
	try
{
	f(hashSize);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheSetRangeForDLP(mclSize hashSize)
{
	return setRangeForDLP(SHE::setRangeForDLP, hashSize);
}
int sheSetRangeForG1DLP(mclSize hashSize)
{
	return setRangeForDLP(SHE::setRangeForG1DLP, hashSize);
}
int sheSetRangeForG2DLP(mclSize hashSize)
{
	return setRangeForDLP(SHE::setRangeForG2DLP, hashSize);
}
int sheSetRangeForGTDLP(mclSize hashSize)
{
	return setRangeForDLP(SHE::setRangeForGTDLP, hashSize);
}

void sheSetTryNum(mclSize tryNum)
{
	SHE::setTryNum(tryNum);
}
void sheUseDecG1ViaGT(int use)
{
	SHE::useDecG1ViaGT(use != 0);
}
void sheUseDecG2ViaGT(int use)
{
	SHE::useDecG2ViaGT(use != 0);
}

template<class HashTable>
mclSize loadTable(HashTable& table, const void *buf, mclSize bufSize)
	try
{
	return table.load(buf, bufSize);
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return 0;
}

mclSize sheLoadTableForG1DLP(const void *buf, mclSize bufSize)
{
	return loadTable(SHE::PhashTbl_, buf, bufSize);
}
mclSize sheLoadTableForG2DLP(const void *buf, mclSize bufSize)
{
	return loadTable(SHE::QhashTbl_, buf, bufSize);
}
mclSize sheLoadTableForGTDLP(const void *buf, mclSize bufSize)
{
	return loadTable(SHE::ePQhashTbl_, buf, bufSize);
}

template<class HashTable>
mclSize saveTable(void *buf, mclSize maxBufSize, const HashTable& table)
	try
{
	return table.save(buf, maxBufSize);
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return 0;
}
mclSize sheSaveTableForG1DLP(void *buf, mclSize maxBufSize)
{
	return saveTable(buf, maxBufSize, SHE::PhashTbl_);
}
mclSize sheSaveTableForG2DLP(void *buf, mclSize maxBufSize)
{
	return saveTable(buf, maxBufSize, SHE::QhashTbl_);
}
mclSize sheSaveTableForGTDLP(void *buf, mclSize maxBufSize)
{
	return saveTable(buf, maxBufSize, SHE::ePQhashTbl_);
}

template<class CT>
int encT(CT *c, const shePublicKey *pub, mclInt m)
	try
{
	cast(pub)->enc(*cast(c), m);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheEncG1(sheCipherTextG1 *c, const shePublicKey *pub, mclInt m)
{
	return encT(c, pub, m);
}

int sheEncG2(sheCipherTextG2 *c, const shePublicKey *pub, mclInt m)
{
	return encT(c, pub, m);
}

int sheEncGT(sheCipherTextGT *c, const shePublicKey *pub, mclInt m)
{
	return encT(c, pub, m);
}

template<class CT, class PK>
int encWithZkpBinT(CT *c, sheZkpBin *zkp, const PK *pub, int m)
	try
{
	cast(pub)->encWithZkpBin(*cast(c), *cast(zkp), m);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheEncWithZkpBinG1(sheCipherTextG1 *c, sheZkpBin *zkp, const shePublicKey *pub, int m)
{
	return encWithZkpBinT(c, zkp, pub, m);
}

int sheEncWithZkpBinG2(sheCipherTextG2 *c, sheZkpBin *zkp, const shePublicKey *pub, int m)
{
	return encWithZkpBinT(c, zkp, pub, m);
}

int shePrecomputedPublicKeyEncWithZkpBinG1(sheCipherTextG1 *c, sheZkpBin *zkp, const shePrecomputedPublicKey *pub, int m)
{
	return encWithZkpBinT(c, zkp, pub, m);
}

int shePrecomputedPublicKeyEncWithZkpBinG2(sheCipherTextG2 *c, sheZkpBin *zkp, const shePrecomputedPublicKey *pub, int m)
{
	return encWithZkpBinT(c, zkp, pub, m);
}

template<class CT>
int decT(mclInt *m, const sheSecretKey *sec, const CT *c)
	try
{
	*m = (cast(sec)->dec)(*cast(c));
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheDecG1(mclInt *m, const sheSecretKey *sec, const sheCipherTextG1 *c)
{
	return decT(m, sec, c);
}

int sheDecG2(mclInt *m, const sheSecretKey *sec, const sheCipherTextG2 *c)
{
	return decT(m, sec, c);
}

int sheDecGT(mclInt *m, const sheSecretKey *sec, const sheCipherTextGT *c)
{
	return decT(m, sec, c);
}

template<class CT>
int decViaGTT(mclInt *m, const sheSecretKey *sec, const CT *c)
	try
{
	*m = (cast(sec)->decViaGT)(*cast(c));
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheDecG1ViaGT(mclInt *m, const sheSecretKey *sec, const sheCipherTextG1 *c)
{
	return decViaGTT(m, sec, c);
}

int sheDecG2ViaGT(mclInt *m, const sheSecretKey *sec, const sheCipherTextG2 *c)
{
	return decViaGTT(m, sec, c);
}


template<class CT>
int isZeroT(const sheSecretKey *sec, const CT *c)
	try
{
	return cast(sec)->isZero(*cast(c));
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return 0;
}

int sheIsZeroG1(const sheSecretKey *sec, const sheCipherTextG1 *c)
{
	return isZeroT(sec, c);
}
int sheIsZeroG2(const sheSecretKey *sec, const sheCipherTextG2 *c)
{
	return isZeroT(sec, c);
}
int sheIsZeroGT(const sheSecretKey *sec, const sheCipherTextGT *c)
{
	return isZeroT(sec, c);
}

template<class CT>
int negT(CT& y, const CT& x)
	try
{
	CT::neg(y, x);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheNegG1(sheCipherTextG1 *y, const sheCipherTextG1 *x)
{
	return negT(*cast(y), *cast(x));
}

int sheNegG2(sheCipherTextG2 *y, const sheCipherTextG2 *x)
{
	return negT(*cast(y), *cast(x));
}

int sheNegGT(sheCipherTextGT *y, const sheCipherTextGT *x)
{
	return negT(*cast(y), *cast(x));
}

template<class CT>
int addT(CT& z, const CT& x, const CT& y)
	try
{
	CT::add(z, x, y);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheAddG1(sheCipherTextG1 *z, const sheCipherTextG1 *x, const sheCipherTextG1 *y)
{
	return addT(*cast(z), *cast(x), *cast(y));
}

int sheAddG2(sheCipherTextG2 *z, const sheCipherTextG2 *x, const sheCipherTextG2 *y)
{
	return addT(*cast(z), *cast(x), *cast(y));
}

int sheAddGT(sheCipherTextGT *z, const sheCipherTextGT *x, const sheCipherTextGT *y)
{
	return addT(*cast(z), *cast(x), *cast(y));
}

template<class CT>
int subT(CT& z, const CT& x, const CT& y)
	try
{
	CT::sub(z, x, y);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheSubG1(sheCipherTextG1 *z, const sheCipherTextG1 *x, const sheCipherTextG1 *y)
{
	return subT(*cast(z), *cast(x), *cast(y));
}

int sheSubG2(sheCipherTextG2 *z, const sheCipherTextG2 *x, const sheCipherTextG2 *y)
{
	return subT(*cast(z), *cast(x), *cast(y));
}

int sheSubGT(sheCipherTextGT *z, const sheCipherTextGT *x, const sheCipherTextGT *y)
{
	return subT(*cast(z), *cast(x), *cast(y));
}

template<class CT1, class CT2, class CT3>
int mulT(CT1& z, const CT2& x, const CT3& y)
	try
{
	CT1::mul(z, x, y);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheMulG1(sheCipherTextG1 *z, const sheCipherTextG1 *x, mclInt y)
{
	return mulT(*cast(z), *cast(x), y);
}

int sheMulG2(sheCipherTextG2 *z, const sheCipherTextG2 *x, mclInt y)
{
	return mulT(*cast(z), *cast(x), y);
}

int sheMulGT(sheCipherTextGT *z, const sheCipherTextGT *x, mclInt y)
{
	return mulT(*cast(z), *cast(x), y);
}

int sheMul(sheCipherTextGT *z, const sheCipherTextG1 *x, const sheCipherTextG2 *y)
{
	return mulT(*cast(z), *cast(x), *cast(y));
}

int sheMulML(sheCipherTextGT *z, const sheCipherTextG1 *x, const sheCipherTextG2 *y)
	try
{
	CipherTextGT::mulML(*cast(z), *cast(x), *cast(y));
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheFinalExpGT(sheCipherTextGT *y, const sheCipherTextGT *x)
	try
{
	CipherTextGT::finalExp(*cast(y), *cast(x));
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

template<class CT>
int reRandT(CT& c, const shePublicKey *pub)
	try
{
	cast(pub)->reRand(c);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheReRandG1(sheCipherTextG1 *c, const shePublicKey *pub)
{
	return reRandT(*cast(c), pub);
}

int sheReRandG2(sheCipherTextG2 *c, const shePublicKey *pub)
{
	return reRandT(*cast(c), pub);
}

int sheReRandGT(sheCipherTextGT *c, const shePublicKey *pub)
{
	return reRandT(*cast(c), pub);
}

template<class CT>
int convert(sheCipherTextGT *y, const shePublicKey *pub, const CT *x)
	try
{
	cast(pub)->convert(*cast(y), *cast(x));
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int sheConvertG1(sheCipherTextGT *y, const shePublicKey *pub, const sheCipherTextG1 *x)
{
	return convert(y, pub, x);
}

int sheConvertG2(sheCipherTextGT *y, const shePublicKey *pub, const sheCipherTextG2 *x)
{
	return convert(y, pub, x);
}

shePrecomputedPublicKey *shePrecomputedPublicKeyCreate()
	try
{
	return reinterpret_cast<shePrecomputedPublicKey*>(new PrecomputedPublicKey());
} catch (...) {
	return 0;
}

void shePrecomputedPublicKeyDestroy(shePrecomputedPublicKey *ppub)
{
	delete cast(ppub);
}

int shePrecomputedPublicKeyInit(shePrecomputedPublicKey *ppub, const shePublicKey *pub)
	try
{
	cast(ppub)->init(*cast(pub));
	return 0;
} catch (...) {
	return 1;
}

template<class CT>
int pEncT(CT *c, const shePrecomputedPublicKey *pub, mclInt m)
	try
{
	cast(pub)->enc(*cast(c), m);
	return 0;
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return -1;
}

int shePrecomputedPublicKeyEncG1(sheCipherTextG1 *c, const shePrecomputedPublicKey *pub, mclInt m)
{
	return pEncT(c, pub, m);
}

int shePrecomputedPublicKeyEncG2(sheCipherTextG2 *c, const shePrecomputedPublicKey *pub, mclInt m)
{
	return pEncT(c, pub, m);
}

int shePrecomputedPublicKeyEncGT(sheCipherTextGT *c, const shePrecomputedPublicKey *pub, mclInt m)
{
	return pEncT(c, pub, m);
}

template<class PK, class CT>
int verifyT(const PK& pub, const CT& c, const ZkpBin& zkp)
	try
{
	return pub.verify(c, zkp);
} catch (std::exception& e) {
#ifndef NDEBUG
	fprintf(stderr, "err %s\n", e.what());
#endif
	return 0;
}

int sheVerifyZkpBinG1(const shePublicKey *pub, const sheCipherTextG1 *c, const sheZkpBin *zkp)
{
	return verifyT(*cast(pub), *cast(c), *cast(zkp));
}
int sheVerifyZkpBinG2(const shePublicKey *pub, const sheCipherTextG2 *c, const sheZkpBin *zkp)
{
	return verifyT(*cast(pub), *cast(c), *cast(zkp));
}
int shePrecomputedPublicKeyVerifyZkpBinG1(const shePrecomputedPublicKey *pub, const sheCipherTextG1 *c, const sheZkpBin *zkp)
{
	return verifyT(*cast(pub), *cast(c), *cast(zkp));
}
int shePrecomputedPublicKeyVerifyZkpBinG2(const shePrecomputedPublicKey *pub, const sheCipherTextG2 *c, const sheZkpBin *zkp)
{
	return verifyT(*cast(pub), *cast(c), *cast(zkp));
}
