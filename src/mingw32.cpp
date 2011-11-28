#include "mingw32.hpp"

char * strndup(const char * s1, size_t n) {

  char * dest = (char *)malloc(n * sizeof(char) + 1);

  memcpy(dest, s1, n);

  dest[n] = 0;

  return dest;
}
