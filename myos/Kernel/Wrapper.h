#pragma once

#include <stdint.h>
#include "multibootdef.h"
class MemMapInfo
{
public:
    uint32_t size()
    {
        return m_size + 1;
    }

    multiboot_mmap_entry operator[](uint32_t index)
    {
        if (index > m_size)
            index = m_size;
        return entries[index];
    }

private:
    uint32_t m_size;
    multiboot_mmap_entry entries[32];
    uint64_t m_totalSize;
    uint64_t m_usableSize;
    friend void InitMemoryMap(MemMapInfo &);
    friend class PageAllocator;
};