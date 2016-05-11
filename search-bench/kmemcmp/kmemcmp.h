#ifndef KMEMCMP_H_
#define KMEMCMP_H_

#include <stddef.h>
#include <stdint.h>

static inline
int kmemcmp(const void *s1, const void *s2, size_t n)
{
  const uint64_t *u1 = (void *)s1;
  const uint64_t *u2 = (void *)s2;
  size_t i;

#define USZ	sizeof(*u1)

  for (i=0; (ssize_t)i<=((ssize_t)(n/USZ))-8; i+=8) {
#define CMP(x)  (u2[i+x] != u1[i+x])
	if (CMP(0) + CMP(1) + CMP(2) + CMP(3)
	     + CMP(4) + CMP(5) + CMP(6) + CMP(7)) return 1;
#undef CMP
  }

  for (; i<n/USZ; i++) {
    if (u1[i] != u2[i]) return 1;
  }

  return i*8 < n && ((u1[i] ^ u2[i]) & (((uint64_t)1 << (8 * (n % USZ))) - 1));
}

#undef USZ

#endif	/* KMEMCMP_H_ */
