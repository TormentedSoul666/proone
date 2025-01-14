#pragma once
/**********************************************************************
* Endianess Independent Byte Extraction
***********************************************************************/
/* prne_getmsbN(x, n)
*
* Extract nth most significant byte of x.
*/
#define prne_getmsb(x, n, w, s)\
	(uint8_t)(((w)(x) & (w)0xFF << (s - 8 * (n))) >> (s - 8 * (n)))
#define prne_getmsb64(x, n) prne_getmsb((x), (n), uint_fast64_t, 56)
#define prne_getmsb32(x, n) prne_getmsb((x), (n), uint_fast32_t, 24)
#define prne_getmsb16(x, n) prne_getmsb((x), (n), uint_fast16_t, 8)

/* prne_recmb_msbN(...)
*
* Recombine bytes in big-endian order to uintN.
*/
#define prne_recmb_msb64(a, b, c, d, e, f, g, h) (\
	((uint_fast64_t)(a) << 56) |\
	((uint_fast64_t)(b) << 48) |\
	((uint_fast64_t)(c) << 40) |\
	((uint_fast64_t)(d) << 32) |\
	((uint_fast64_t)(e) << 24) |\
	((uint_fast64_t)(f) << 16) |\
	((uint_fast64_t)(g) << 8) |\
	((uint_fast64_t)(h) << 0)\
)
#define prne_recmb_msb32(a, b, c, d) (\
	((uint_fast32_t)(a) << 24) |\
	((uint_fast32_t)(b) << 16) |\
	((uint_fast32_t)(c) << 8) |\
	((uint_fast32_t)(d) << 0)\
)
#define prne_recmb_msb16(a, b) (\
	((uint_fast16_t)(a) << 8) |\
	((uint_fast16_t)(b) << 0)\
)

/* Machine Characteristics
*/
#define PRNE_ENDIAN_LITTLE 1
#define PRNE_ENDIAN_BIG 2

#ifdef __GNUC__
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		#define PRNE_HOST_ENDIAN PRNE_ENDIAN_BIG
	#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		#define PRNE_HOST_ENDIAN PRNE_ENDIAN_LITTLE
	#else
		#error "FIXME!"
	#endif
#else
	#error "FIXME!"
#endif

#define prne_einv16(x) (((0xFF00 & x) >> 8) | ((0x00FF & x) << 8))

#if PRNE_HOST_ENDIAN == PRNE_ENDIAN_BIG
#define prne_htobe16(x) (x)
#define prne_be16toh(x) (x)
#define prne_htole16(x) prne_einv16(x)
#define prne_le16toh(x) prne_einv16(x)
#elif PRNE_HOST_ENDIAN == PRNE_ENDIAN_LITTLE
#define prne_htobe16(x) prne_einv16(x)
#define prne_be16toh(x) prne_einv16(x)
#define prne_htole16(x) (x)
#define prne_le16toh(x) (x)
#else
#endif
