#pragma once
#include <stdint.h>
#include <stddef.h>

static void hlt(void)
{
    for (;;)
    {
        __asm__("hlt");
    }
}
size_t strlen(const char *s);
void memset(uint8_t *ptr, size_t size, uint8_t val);