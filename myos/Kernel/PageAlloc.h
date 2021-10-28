#pragma once

#include "bitmap.h"
#define PAGE_SIZE 4096

class PageAllocator
{

public:
    PageAllocator(MemMapInfo &);
    void LockPage(void *);
    void FreePage(void *);
    void FreePages(void *, uint64_t);
    void LockPages(void *, uint64_t);
    uint64_t GetFreeRam();
    uint64_t GetReservedRam();
    uint64_t GetUsedRam();

private:
    bitmap map;
    MemMapInfo meminfo;
    static uint64_t freeMemory, usedMemory, reservedMemory;

private:
    static void InitBitMap(bitmap &);
    void ReservePage(void *);
    void UnreservePage(void *);
    void UnreservePages(void *, uint64_t);
    void ReservePages(void *, uint64_t);
};