//===-- sanitizer_libc.cc -------------------------------------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is shared between AddressSanitizer and ThreadSanitizer
// run-time libraries. See sanitizer_libc.h for details.
//===----------------------------------------------------------------------===//
#include "sanitizer_common.h"
#include "sanitizer_libc.h"

namespace __sanitizer {

s64 internal_atoll(const char *nptr) {
  return internal_simple_strtoll(nptr, (char**)0, 10);
}

void *internal_memchr(const void *s, int c, uptr n) {
  const char* t = (char*)s;
  for (uptr i = 0; i < n; ++i, ++t)
    if (*t == c)
      return (void*)t;
  return 0;
}

int internal_memcmp(const void* s1, const void* s2, uptr n) {
  const char* t1 = (char*)s1;
  const char* t2 = (char*)s2;
  for (uptr i = 0; i < n; ++i, ++t1, ++t2)
    if (*t1 != *t2)
      return *t1 < *t2 ? -1 : 1;
  return 0;
}

void *internal_memcpy(void *dest, const void *src, uptr n) {
  char *d = (char*)dest;
  char *s = (char*)src;
  for (uptr i = 0; i < n; ++i)
    d[i] = s[i];
  return dest;
}

void *internal_memmove(void *dest, const void *src, uptr n) {
  char *d = (char*)dest;
  char *s = (char*)src;
  sptr i, signed_n = (sptr)n;
  CHECK_GE(signed_n, 0);
  if (d < s) {
    for (i = 0; i < signed_n; ++i)
      d[i] = s[i];
  } else {
    if (d > s && signed_n > 0)
      for (i = signed_n - 1; i >= 0 ; --i) {
        d[i] = s[i];
      }
  }
  return dest;
}

void *internal_memset(void* s, int c, uptr n) {
  // The next line prevents Clang from making a call to memset() instead of the
  // loop below.
  // FIXME: building the runtime with -ffreestanding is a better idea. However
  // there currently are linktime problems due to PR12396.
  char volatile *t = (char*)s;
  for (uptr i = 0; i < n; ++i, ++t) {
    *t = c;
  }
  return s;
}

uptr internal_strcspn(const char *s, const char *reject) {
  uptr i;
  for (i = 0; s[i]; i++) {
    if (internal_strchr(reject, s[i]) != 0)
      return i;
  }
  return i;
}

char* internal_strdup(const char *s) {
  uptr len = internal_strlen(s);
  char *s2 = (char*)InternalAlloc(len + 1);
  internal_memcpy(s2, s, len);
  s2[len] = 0;
  return s2;
}

int internal_strcmp(const char *s1, const char *s2) {
  while (true) {
    unsigned c1 = *s1;
    unsigned c2 = *s2;
    if (c1 != c2) return (c1 < c2) ? -1 : 1;
    if (c1 == 0) break;
    s1++;
    s2++;
  }
  return 0;
}

int internal_strncmp(const char *s1, const char *s2, uptr n) {
  for (uptr i = 0; i < n; i++) {
    unsigned c1 = *s1;
    unsigned c2 = *s2;
    if (c1 != c2) return (c1 < c2) ? -1 : 1;
    if (c1 == 0) break;
    s1++;
    s2++;
  }
  return 0;
}

char* internal_strchr(const char *s, int c) {
  while (true) {
    if (*s == (char)c)
      return (char*)s;
    if (*s == 0)
      return 0;
    s++;
  }
}

char *internal_strrchr(const char *s, int c) {
  const char *res = 0;
  for (uptr i = 0; s[i]; i++) {
    if (s[i] == c) res = s + i;
  }
  return (char*)res;
}

uptr internal_strlen(const char *s) {
  uptr i = 0;
  while (s[i]) i++;
  return i;
}

char *internal_strncat(char *dst, const char *src, uptr n) {
  uptr len = internal_strlen(dst);
  uptr i;
  for (i = 0; i < n && src[i]; i++)
    dst[len + i] = src[i];
  dst[len + i] = 0;
  return dst;
}

char *internal_strncpy(char *dst, const char *src, uptr n) {
  uptr i;
  for (i = 0; i < n && src[i]; i++)
    dst[i] = src[i];
  for (; i < n; i++)
    dst[i] = '\0';
  return dst;
}

uptr internal_strnlen(const char *s, uptr maxlen) {
  uptr i = 0;
  while (i < maxlen && s[i]) i++;
  return i;
}

char *internal_strstr(const char *haystack, const char *needle) {
  // This is O(N^2), but we are not using it in hot places.
  uptr len1 = internal_strlen(haystack);
  uptr len2 = internal_strlen(needle);
  if (len1 < len2) return 0;
  for (uptr pos = 0; pos <= len1 - len2; pos++) {
    if (internal_memcmp(haystack + pos, needle, len2) == 0)
      return (char*)haystack + pos;
  }
  return 0;
}

s64 internal_simple_strtoll(const char *nptr, char **endptr, int base) {
  CHECK_EQ(base, 10);
  while (IsSpace(*nptr)) nptr++;
  int sgn = 1;
  u64 res = 0;
  bool have_digits = false;
  char *old_nptr = (char*)nptr;
  if (*nptr == '+') {
    sgn = 1;
    nptr++;
  } else if (*nptr == '-') {
    sgn = -1;
    nptr++;
  }
  while (IsDigit(*nptr)) {
    res = (res <= UINT64_MAX / 10) ? res * 10 : UINT64_MAX;
    int digit = ((*nptr) - '0');
    res = (res <= UINT64_MAX - digit) ? res + digit : UINT64_MAX;
    have_digits = true;
    nptr++;
  }
  if (endptr != 0) {
    *endptr = (have_digits) ? (char*)nptr : old_nptr;
  }
  if (sgn > 0) {
    return (s64)(Min((u64)INT64_MAX, res));
  } else {
    return (res > INT64_MAX) ? INT64_MIN : ((s64)res * -1);
  }
}

bool mem_is_zero(const char *beg, uptr size) {
  CHECK_LE(size, 1UL << FIRST_32_SECOND_64(30, 40));  // Sanity check.
  const char *end = beg + size;
  uptr *aligned_beg = (uptr *)RoundUpTo((uptr)beg, sizeof(uptr));
  uptr *aligned_end = (uptr *)RoundDownTo((uptr)end, sizeof(uptr));
  uptr all = 0;
  // Prologue.
  for (const char *mem = beg; mem < (char*)aligned_beg && mem < end; mem++)
    all |= *mem;
  // Aligned loop.
  for (; aligned_beg < aligned_end; aligned_beg++)
    all |= *aligned_beg;
  // Epilogue.
  if ((char*)aligned_end >= beg)
    for (const char *mem = (char*)aligned_end; mem < end; mem++)
      all |= *mem;
  return all == 0;
}

}  // namespace __sanitizer
