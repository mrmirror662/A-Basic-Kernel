#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <limine.h>

#include "util.h"

void init_term();
void print(const char *s);
void print(uint64_t i);
void print(void *ptr);
void printc(uint8_t c);
void printf(const char *, ...);