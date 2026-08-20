#ifndef KSTRING_H
#define KSTRING_H

/* Freestanding headers provided by GCC/Clang in bare-metal mode */
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Copies n bytes from memory area src to memory area dest.
 * Memory areas MUST NOT overlap.
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * Fills the first n bytes of the memory area pointed to by dest
 * with the constant byte c.
 */
void *memset(void *dest, int c, size_t n);

/**
 * Compares the first n bytes of memory areas s1 and s2.
 * Returns 0 if identical, <0 if s1 < s2, or >0 if s1 > s2.
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * Copies n bytes from memory area src to memory area dest.
 * Memory areas MAY overlap.
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * Returns the length of a null-terminated string.
 */
size_t strlen(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* KSTRING_H */
