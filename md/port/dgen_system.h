#ifndef DGEN_SYSTEM_H
#define DGEN_SYSTEM_H

#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#define DGEN_VER "dgen-sdl-1.33"

uint8_t *load(size_t *file_size, const char *name, size_t max_size);
void unload(uint8_t *data);
void dump_z80ram(unsigned char *z80ram, int size);
void DGEN_Wait(void);

#define elemof(a) (sizeof(a) / sizeof((a)[0]))
#define containerof(p, s, m) (s *)((uintptr_t)(p)-offsetof(s, m))

#define BITS_TO_BYTES(v) ((((v) + 7u) & ~7u) >> 3)

#define le2h16(v) h2le16(v)
static inline uint16_t h2le16(uint16_t v) {
#ifdef WORDS_BIGENDIAN
	return ((v >> 8) | (v << 8));
#else
	return v;
#endif
}

#define be2h16(v) h2be16(v)
static inline uint16_t h2be16(uint16_t v) {
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return ((v >> 8) | (v << 8));
#endif
}

#define le2h32(v) h2le32(v)
static inline uint32_t h2le32(uint32_t v) {
#ifdef WORDS_BIGENDIAN
	return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) |
			((v & 0x0000ff00) << 8) | ((v & 0x000000ff) << 24));
#else
	return v;
#endif
}

#define be2h32(v) h2be32(v)
static inline uint32_t h2be32(uint32_t v) {
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >> 8) |
			((v & 0x0000ff00) << 8) | ((v & 0x000000ff) << 24));
#endif
}

typedef uint8_t uint24_t[3];

static inline uint24_t *u24cpy(uint24_t *dst, const uint24_t *src) {
	/* memcpy() is sometimes faster. */
#ifdef U24CPY_MEMCPY
	memcpy(*dst, *src, sizeof(*dst));
#else
	(*dst)[0] = (*src)[0];
	(*dst)[1] = (*src)[1];
	(*dst)[2] = (*src)[2];
#endif
	return dst;
}

extern char **complete_path(const char *prefix, size_t len,
							const char *relative);
extern void complete_path_free(char **cp);

static inline size_t strcommon(const char *s1, const char *s2) {
	size_t i;

	for (i = 0; ((s1[i] != '\0') && (s2[i] != '\0')); ++i)
		if (s1[i] != s2[i])
			break;
	return i;
}

#define BACKSLASHIFY_NOQUOTES 0x0001

extern char *backslashify(const uint8_t *src, size_t size, unsigned int flags,
						  size_t *pos);

extern size_t utf8u32(uint32_t *u32, const uint8_t *u8);
extern size_t utf32u8(uint8_t *u8, uint32_t u32);

extern int prefix_casematch(const char *str, const char *argv[]);
extern size_t prefix_getuint(const char *str, unsigned int *num);

#endif
