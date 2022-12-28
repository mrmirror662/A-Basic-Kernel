
#include "util.h"

size_t strlen(const char *s)
{
    size_t i = 0;
    while (s[i] != 0)
        i++;
    return i;
}
void memset(uint8_t *ptr, size_t size, uint8_t val)
{
    for (size_t i = 0; i < size; i++)
        ptr[i] = val;
}