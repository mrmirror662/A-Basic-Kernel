#include "PageAlloc.h"
#include "Print.h"
uint64_t PageAllocator::usedMemory;
uint64_t PageAllocator::freeMemory;
uint64_t PageAllocator::reservedMemory;

PageAllocator::PageAllocator(MemMapInfo &_meminfo)
{
    Console c;
    meminfo = _meminfo;
    uint64_t max = 0;
    void *addr = nullptr;
    for (uint64_t i = 0; i < meminfo.size(); i++)
    {
        if (max < meminfo[i].len && meminfo[i].type == MULTIBOOT_MEMORY_AVAILABLE)
        {
            addr = (void *)meminfo[i].addr;
            max = meminfo[i].len;
        }
    }
    usedMemory = 0;
    reservedMemory = 0;
    freeMemory = meminfo.m_totalSize;

    map.buffer = (uint8_t *)addr + 30000000;
    map.m_size = ((meminfo.m_totalSize / PAGE_SIZE) / 8) + 1;
    c.logf("addr:%x m_size:%i total size:%i usable size:%i\n", addr, map.m_size, meminfo.m_totalSize, meminfo.m_usableSize);
    InitBitMap(map);
    //   LockPages(map.buffer, map.m_size / PAGE_SIZE);

    // for (int i = 0; i < meminfo.size(); i++)
    // {
    //     auto curr = meminfo[i];
    //     if (curr.type != MULTIBOOT_MEMORY_AVAILABLE)
    //     {
    //         ReservePages((void *)curr.addr, curr.len / PAGE_SIZE);
    //     }
    // }
}
void PageAllocator::InitBitMap(bitmap &map)
{
    Console c;

    for (uint64_t i = 0; i < 1; i++)
    {
        map.buffer[i] = 1;
        // c.log(map.buffer[i]);
    }
}

void PageAllocator::FreePage(void *addr)
{
    auto index = ((uint64_t)(addr)) / 8;
    map.set(index, false);
    freeMemory += PAGE_SIZE;
    usedMemory -= PAGE_SIZE;
    return;
}

void PageAllocator::LockPage(void *addr)
{
    auto index = ((uint64_t)(addr)) / 8;
    map.set(index, true);
    freeMemory -= PAGE_SIZE;
    usedMemory += PAGE_SIZE;
    return;
}
void PageAllocator::FreePages(void *addr, uint64_t numPages)
{
    for (uint64_t i = 0; i < numPages; i++)
    {
        FreePage(addr + (i * PAGE_SIZE));
    }
}
void PageAllocator::LockPages(void *addr, uint64_t numPages)
{
    for (uint64_t i = 0; i < numPages; i++)
    {
        LockPage(addr + (i * PAGE_SIZE));
    }
}
void PageAllocator::ReservePages(void *addr, uint64_t numPages)
{
    for (uint64_t i = 0; i < numPages; i++)
    {
        ReservePage(addr + (i * PAGE_SIZE));
    }
}
void PageAllocator::UnreservePages(void *addr, uint64_t numPages)
{
    for (uint64_t i = 0; i < numPages; i++)
    {
        UnreservePage(addr + (i * PAGE_SIZE));
    }
}
void PageAllocator::UnreservePage(void *addr)
{
    auto index = ((uint64_t)(addr)) / 8;
    map.set(index, false);
    freeMemory += PAGE_SIZE;
    reservedMemory -= PAGE_SIZE;
    return;
}
void PageAllocator::ReservePage(void *addr)
{
    auto index = ((uint64_t)(addr)) / 8;
    map.set(index, false);
    freeMemory -= PAGE_SIZE;
    reservedMemory += PAGE_SIZE;
    return;
}

uint64_t PageAllocator::GetFreeRam()
{
    return freeMemory;
}
uint64_t PageAllocator::GetUsedRam()
{
    return usedMemory;
}
uint64_t PageAllocator::GetReservedRam()
{
    return reservedMemory;
}